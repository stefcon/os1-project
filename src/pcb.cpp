#include <dos.h>
#include <string.h>
#include <assert.h>
#include "pcb.h"
#include "lock.h"
#include "utils.h"
#include "system.h"
#include "SCHEDULE.H"

ID PCB::threadID = 0;
volatile PCB* volatile PCB::running = nullptr;
volatile PCB* fork_child = nullptr;
volatile PCB* fork_parent = nullptr;
List<PCB*> PCB::all_pcbs_;


PCB::PCB(StackSize stack_size, Time time_slice, Thread* my_thread)
: time_slice_(time_slice), my_thread_(my_thread), state_(Initialized) {

	LOCK
	my_id_ = threadID++;
	UNLOCK

	initializeStack(stack_size);

	LOCK
	all_pcbs_.push_back(this);
	UNLOCK
}

PCB::~PCB() {
	LOCK
	if (stack_ != nullptr) {
		delete[] stack_;
		stack_ = nullptr;
	}
	my_thread_ = nullptr;

	List<PCB*>::Iterator to_remove;
	for (to_remove = all_pcbs_.begin(); *to_remove != this; ++to_remove);
	PCB::all_pcbs_.remove_iterator(to_remove);
	UNLOCK
}

void PCB::initializeStack(StackSize stack_size) {
	if (stack_size > max_stack_size) stack_size = max_stack_size;

	if (stack_size == 0) {
		sp_ = ss_ = bp_ = 0;
		stack_ = nullptr;
		return;
	}
	stack_size_ = stack_size;
	stack_size /= sizeof(unsigned);

	LOCK
	stack_ = new unsigned[stack_size];
	UNLOCK
	if (stack_ == nullptr) {
		return;
	}
	stack_[stack_size - 1] = 0x200;			// set I flag in starting PSW for the thread
#ifndef BCC_BLOCK_IGNORE
	stack_[stack_size - 2] = FP_SEG(PCB::wrapper);
	stack_[stack_size - 3] = FP_OFF(PCB::wrapper);

	sp_ = FP_OFF(stack_ + stack_size - 12);	// Reserve space for saving other registers on the stack
	ss_ = FP_SEG(stack_ + stack_size - 12);
	bp_ = FP_OFF(stack_ + stack_size - 12);
#endif
	stack_[stack_size - 12] = 0;						// Starting value of bp, important for fork
}

ID PCB::get_id() const volatile {
	return my_id_;
}

ID PCB::get_running_id() {
	if (running)
		return running->my_id_;
	else return -1;
}

PCB* PCB::get_pcb_by_id(ID id) {
	Thread* thread = PCB::get_thread_by_id(id);
	return thread->my_pcb_;
}

Thread* PCB::get_thread_by_id(ID id) {
	Thread* return_value = nullptr;

	LOCK
	List<PCB*>::Iterator p;
	for (p = all_pcbs_.begin(); p != all_pcbs_.end(); ++p) {
		if ((*p)->my_id_ == id) {
			return_value = (*p)->my_thread_;
			break;
		}
	}
	UNLOCK
	return return_value;
}

void PCB::set_state(State new_state) volatile {
	LOCK
	assert(state_ != PCB::Terminated);
	state_ = new_state;
	UNLOCK
}

PCB::State PCB::get_state() const volatile {
	return state_;
}

unsigned PCB::get_time_slice() const volatile {
	return time_slice_;
}

void PCB::wrapper() {
	PCB::running->my_thread_->run();
	PCB::exit();
}

void PCB::start() {
	LOCK
	if (state_ == Initialized) {
		state_ = Ready;
		Scheduler::put(this);
	}
	UNLOCK
}

void PCB::waitToComplete() {
	LOCK
	if (PCB::running != this
			&& state_ != Initialized
			&& state_ != Terminated
			&& this != System::main_pcb_
			&& this != System::idle_pcb_) {
		PCB::running->state_ = Suspended;
		suspended_list_.push_front((PCB*)PCB::running);
		UNLOCK
		dispatch();
	}
	else
		UNLOCK

}

void PCB::waitForForkChildren() {
	List<PCB*>::Iterator child = PCB::running->children_list_.begin();
	while (child != PCB::running->children_list_.end()) {
		(*child)->waitToComplete();
		child = PCB::running->children_list_.remove_iterator(child);
	}
}

void PCB::exit() {
	LOCK
	while (((PCB*)PCB::running)->suspended_list_.empty() == false) {
		PCB* suspended_thread = ((PCB*)(PCB::running))->suspended_list_.back();
		((PCB*)PCB::running)->suspended_list_.pop_back();

		if (suspended_thread != nullptr && suspended_thread->get_state() != Terminated) {
			suspended_thread->set_state(Ready);
			Scheduler::put(suspended_thread);
		}
	}
	PCB::running->set_state(Terminated);
	UNLOCK

	dispatch();
}


void interrupt PCB::fork() {

	memcpy((void*)fork_child->stack_,
			(void*)fork_parent->stack_,
			(size_t)fork_parent->stack_size_);


	unsigned parent_ss, parent_bp;
	unsigned child_stack_offset, parent_stack_offset, displacement;
#ifndef BCC_BLOCK_IGNORE
	asm {
		mov parent_ss, ss
		mov parent_bp, bp
	}

	child_stack_offset = FP_OFF(fork_child->stack_);
	parent_stack_offset = FP_OFF(fork_parent->stack_);
#endif

	displacement = parent_stack_offset - child_stack_offset;

	fork_child->sp_ = fork_child->bp_ = parent_bp - displacement;

	unsigned index;
	while (true) {
		index = (parent_bp - parent_stack_offset) / sizeof(unsigned);

		if (fork_parent->stack_[index] == 0) {
			fork_child->stack_[index] = 0;
			break;
		}

		fork_child->stack_[index] = fork_parent->stack_[index] - displacement;

		parent_bp = fork_parent->stack_[index];

	}

	fork_child->my_thread_->start();

}
