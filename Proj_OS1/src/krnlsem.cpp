#include "krnlsem.h"
#include "SCHEDULE.H"
#include "lock.h"
#include "utils.h"


List<KernelSem*> KernelSem::all_semaphores_;

unsigned KernelSem::global_tick_counter_ = 0;


KernelSem::KernelSem(int undo, int init) : val_(init<0? 0:init), undo_(undo) {

	KernelSem::all_semaphores_.push_back(this);

}


KernelSem::~KernelSem() {

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

	dispatch();
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
	HARD_LOCK
	// Modif
	if (undo_ != 0) {
		PCB::running->register_wait(this);
	}
	if (--val_ < 0) {
		block(max_time_to_wait, wait_return_val);
	} else {
		wait_return_val = 1;
	}
	HARD_UNLOCK
	return wait_return_val;
}


void KernelSem::signal() {
	HARD_LOCK
	// Modif
	if (undo_ != 0) {
		PCB::running->register_signal(this);
	}
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



