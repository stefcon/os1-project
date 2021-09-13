#include "pcb.h"
#include "lock.h"
#include "timer.h"
#include "utils.h"


Thread::Thread(StackSize stack_size, Time time_slice) {
	HARD_LOCK
	my_pcb_ = new PCB(stack_size, time_slice, this);
	HARD_UNLOCK
}


Thread::~Thread() {
	HARD_LOCK
	delete my_pcb_;
	my_pcb_ = nullptr;
	HARD_UNLOCK
}


void Thread::start() {
	if (my_pcb_)
		my_pcb_->start();
}


void Thread::waitToComplete() {
	if (my_pcb_) {
		my_pcb_->waitToComplete();
	}
}


ID Thread::getId() {
	if (my_pcb_)
		return my_pcb_->get_id();
	return -1;
}


ID Thread::getRunningId() {
	return PCB::get_running_id();
}


Thread* Thread::getThreadById(ID id) {
	return PCB::get_thread_by_id(id);
}


void dispatch() {
#ifndef BCC_BLOCK_IGNORE
	HARD_LOCK
	context_switch_wanted = true;
	timer();
	HARD_UNLOCK
#endif
}


Thread* Thread::clone() const {
	return new Thread(my_pcb_->stack_size_, my_pcb_->time_slice_);
}


void Thread::exit() {
	PCB::exit();
}


void Thread::waitForForkChildren() {
	PCB::waitForForkChildren();
}

// Modif
void Thread::pair(Semaphore* sem, Thread* t1, Thread* t2) {
	PCB::pair(sem, t1, t2);
}


ID Thread::fork() {
	LOCK

	Thread* child = nullptr;
	// We can only fork threads that have allocated their stack
	if (PCB::running->stack_size_ != 0) {
		child = PCB::running->my_thread_->clone();
		if (!child || !child->my_pcb_ || child->getId() == -1) {
			delete child;
			UNLOCK
			return -1;
		}
	} else {
		UNLOCK
		return -1;
	}


	fork_child = child->my_pcb_;
	fork_parent = (PCB*)PCB::running;

	PCB::fork();

	if (child->my_pcb_ == PCB::running) {
		return 0;
	} else {
		UNLOCK
		return child->getId();
	}

}


