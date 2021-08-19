#include "pcb.h"
#include "lock.h"
#include "timer.h"
#include "utils.h"
#include <assert.h>
#include <iostream.h>


Thread::Thread(StackSize stack_size, Time time_slice) {
	LOCK
	my_pcb_ = new PCB(stack_size, time_slice, this);
	UNLOCK
}


Thread::~Thread() {
	LOCK
	delete my_pcb_;
	my_pcb_ = nullptr;
	UNLOCK;
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


ID Thread::fork() {
	LOCK

	Thread* child = nullptr;
	// We can only fork threads that have allocated their stack
	if (PCB::running->stack_size_ != 0) {
		child = PCB::running->my_thread_->clone();
		if (child == nullptr || child->my_pcb_->stack_ == nullptr) {
			UNLOCK
			return -1;
		}

	} else  {
		UNLOCK
		return -1;
	}

	PCB::running->children_list_.push_back(child->my_pcb_);

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


