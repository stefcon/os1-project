#include "krnlev.h"
#include "ivtentry.h"
#include "pcb.h"
#include "lock.h"
#include "SCHEDULE.H"


KernelEv::KernelEv(IVTNo ivt_no) : ivt_no_(ivt_no), owner_((PCB*)PCB::running), blocked_(0) ,val_(0) {

	IVTEntry::ivt_entry_table_[ivt_no_]->set_event(this);

}


KernelEv::~KernelEv() {

	IVTEntry::ivt_entry_table_[ivt_no_]->remove_event();
	this->signal();

}

bool KernelEv::block() {
	if (PCB::running == owner_) {
		if (--val_ < 0) {
			PCB::running->set_state(PCB::Suspended);
			blocked_ = (PCB*)PCB::running;
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

void KernelEv::wait() {
#ifndef BCC_BLOCK_IGNORE
	HARD_LOCK
	if (block()) {
		dispatch();
	}
	HARD_UNLOCK
#endif
}


void KernelEv::signal() {
	#ifndef BCC_BLOCK_IGNORE
	HARD_LOCK
	#endif
	if (++val_ <= 0 && blocked_ != nullptr) {
		blocked_->set_state(PCB::Ready);
		Scheduler::put(blocked_);
		blocked_ = nullptr;
	} else {
		val_ = 1;
	}
	#ifndef BCC_BLOCK_IGNORE
	HARD_UNLOCK
	#endif
}
