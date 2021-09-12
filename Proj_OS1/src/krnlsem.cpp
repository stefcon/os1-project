#include "krnlsem.h"
#include "SCHEDULE.H"
#include "lock.h"
#include "utils.h"
#include <stdio.h>


List<KernelSem*> KernelSem::all_semaphores_;

unsigned KernelSem::global_tick_counter_ = 0;


KernelSem::KernelSem(int init) : val_(init<0? 0:init), tick_counter_(0), curr_lock_(Free),
	lock_val_(0) {
	LOCK
	KernelSem::all_semaphores_.push_back(this);
	UNLOCK
}


KernelSem::~KernelSem() {
	LOCK
	remove_from_all_semaphores();

	List<BlockedInfo*>::Iterator to_deblock;
	while (unlimited_blocked_list_.empty() == false) {
		to_deblock = unlimited_blocked_list_.begin();
		deblock(to_deblock, 0, Unlimited);
	}

	while (sleep_blocked_list_.empty() == false) {
		to_deblock = sleep_blocked_list_.begin();
		deblock(to_deblock, 0, Sleep);
	}

	UNLOCK
}


void KernelSem::remove_from_all_semaphores() {
	List<KernelSem*>::Iterator semaphore = KernelSem::all_semaphores_.begin();
	for (; semaphore != KernelSem::all_semaphores_.end(); ++semaphore) {
		if (*semaphore == this) {
			KernelSem::all_semaphores_.remove_iterator(semaphore);
			break;
		}
	}
}


void KernelSem::block(Time max_time_to_wait, volatile int& wait_return_val) {
	BlockedInfo* blocked_info = new BlockedInfo((PCB*)PCB::running, max_time_to_wait, wait_return_val);
	PCB::running->set_state(PCB::Suspended);

	if (max_time_to_wait == 0)
		unlimited_blocked_list_.push_back(blocked_info);
	else
		insert_sleep_sorted(blocked_info);
	UNLOCK

	dispatch();

	LOCK
}


void KernelSem::deblock(List<BlockedInfo*>::Iterator& sem_node, int wait_return_val, ListType list_type) {
	LOCK
	(*sem_node)->wait_return_val = wait_return_val;
	(*sem_node)->pcb->set_state(PCB::Ready);
	Scheduler::put((*sem_node)->pcb);


	List<BlockedInfo*>::Iterator to_remove = sem_node++;

	if (sem_node != sleep_blocked_list_.end() && sem_node != unlimited_blocked_list_.end()) {
		(*sem_node)->time_to_wait += (*to_remove)->time_to_wait;
	}

	delete *to_remove;

	if (list_type == Unlimited)
		unlimited_blocked_list_.remove_iterator(to_remove);
	else if (list_type == Sleep)
		sleep_blocked_list_.remove_iterator(to_remove);
	UNLOCK
}


int KernelSem::wait(Time max_time_to_wait) {
	volatile int wait_return_val;
	LOCK
	if (--val_ < 0) {
		block(max_time_to_wait, wait_return_val);
	} else {
		wait_return_val = 1;
	}
	UNLOCK
	return wait_return_val;
}


void KernelSem::signal() {
	HARD_LOCK
	if (val_++ < 0) {
		List<BlockedInfo*>::Iterator to_deblock;

		if (unlimited_blocked_list_.empty() == false) {
			to_deblock = unlimited_blocked_list_.begin();
			deblock(to_deblock, 1, Unlimited);
		}
		else {
			to_deblock = sleep_blocked_list_.begin();
			deblock(to_deblock, 1, Sleep);
		}

	}
	HARD_UNLOCK
}


void KernelSem::insert_sleep_sorted(BlockedInfo* blocked_info) {
	List<BlockedInfo*>::Iterator iter = sleep_blocked_list_.begin();
	for (; iter != sleep_blocked_list_.end(); ++iter) {
		if (blocked_info->time_to_wait < (*iter)->time_to_wait) {

			(*iter)->time_to_wait -= blocked_info->time_to_wait;
			sleep_blocked_list_.insert(iter, blocked_info);
			return;
		} else {
			blocked_info->time_to_wait -= (*iter)->time_to_wait;
		}
	}

	sleep_blocked_list_.push_back(blocked_info);
}

void KernelSem::tickAllSemaphores() {
	++global_tick_counter_;

	if (lock_counter == 0) {
		List<KernelSem*>::Iterator semaphore = KernelSem::all_semaphores_.begin();
		for (; semaphore != KernelSem::all_semaphores_.end(); ++semaphore) {

			unsigned tick_counter = global_tick_counter_;

			List<BlockedInfo*>::Iterator sem_node = (*semaphore)->sleep_blocked_list_.begin();
			while (sem_node != (*semaphore)->sleep_blocked_list_.end() && tick_counter) {

				if ((*sem_node)->time_to_wait <= tick_counter) {
					tick_counter -= (*sem_node)->time_to_wait;
					(*sem_node)->time_to_wait = 0;

					// Deblock all neighboring threads whose time_to_wait == 0
					while (sem_node != (*semaphore)->sleep_blocked_list_.end() &&
							(*sem_node)->time_to_wait == 0) {
						(*semaphore)->val_++;
						(*semaphore)->deblock(sem_node, 0, Sleep);
					}
				} else {
					(*sem_node)->time_to_wait -= tick_counter;
					tick_counter = 0;
				}
			}

		}
		global_tick_counter_ = 0;
	}
}


int KernelSem::val() const {
	return val_;
}

// Modif
void KernelSem::open(char c) {
	LOCK
	if (c == 'r') {
		if (curr_lock_ == Write) {
			// Exclusive key is held by another thread, needs to get blocked
			LockInfo* lock_info = new LockInfo((PCB*)PCB::running, Read);
			PCB::running->set_state(PCB::Suspended);
			lock_blocked_list_.push_back(lock_info);
			UNLOCK
			dispatch();
			LOCK
		}
		else {
			curr_lock_ = Read;
			++lock_val_;
		}
	}
	else if (c == 'w') {
		if (curr_lock_ == Write || curr_lock_ == Read) {
			LockInfo* lock_info = new LockInfo((PCB*)PCB::running, Write);
			PCB::running->set_state(PCB::Suspended);
			lock_blocked_list_.push_back(lock_info);
			UNLOCK
			dispatch();
			LOCK
		}
		else {
			curr_lock_ = Write;
			lock_val_ = 1;
		}
	}
	UNLOCK
}


void KernelSem::close() {
	LOCK
	if (curr_lock_ == Read) {
		--lock_val_;
		if (lock_val_ == 0 && lock_blocked_list_.empty() == false) {
			// We have "Write" threads waiting in the liste
			LockInfo* lock_info = lock_blocked_list_.front();
			lock_blocked_list_.pop_front();

			lock_info->pcb->set_state(PCB::Ready);
			Scheduler::put(lock_info->pcb);

			curr_lock_ = Write;
			++lock_val_;
		}
	}
	else if (curr_lock_ == Write) {
		--lock_val_;
		if (lock_blocked_list_.empty() == false) {

			LockInfo* lock_info = lock_blocked_list_.front();

			if (lock_info->lock == Write) {
				lock_blocked_list_.pop_front();

				lock_info->pcb->set_state(PCB::Ready);
				Scheduler::put(lock_info->pcb);
				delete lock_info;

				curr_lock_ = Write;
				++lock_val_;
			}
			else {
				curr_lock_ = Read;

				List<LockInfo*>::Iterator iter = lock_blocked_list_.begin();
				while (iter != lock_blocked_list_.end()) {
					if ((*iter)->lock == Read) {

						(*iter)->pcb->set_state(PCB::Ready);
						Scheduler::put((*iter)->pcb);
						delete *iter;

						++lock_val_;
						lock_blocked_list_.remove_iterator(iter);
					}
					else {
						++iter;
					}
				}
			}
		}
		else {
			curr_lock_ = Free;
			lock_val_ = 0;
		}
	}
	UNLOCK
}



