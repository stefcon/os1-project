#include "krnlev.h"
#include "ivtentry.h"
#include "pcb.h"
#include "lock.h"
#include "SCHEDULE.H"
#include <dos.h>
#include <iostream.h>

KernelEv::KernelEv(IVTNo ivt_no) : ivt_no_(ivt_no), owner_((PCB*)PCB::running), blocked_(0) ,val_(0) {

	IVTEntry::ivt_entry_table_[ivt_no_]->set_event(this);

}

KernelEv::~KernelEv() {

	IVTEntry::ivt_entry_table_[ivt_no_]->remove_event();
	releaseOwner();

}

void KernelEv::releaseOwner() {
	LOCK
	if (blocked_ != nullptr
			&& blocked_->get_state() == PCB::Suspended) {
		blocked_->set_state(PCB::Ready);
		Scheduler::put(blocked_);
		blocked_ = nullptr;
	}
	UNLOCK
}

void KernelEv::wait() {
	LOCK
	if (PCB::running == owner_) {
		if (--val_ < 0) {
			PCB::running->set_state(PCB::Suspended);
			blocked_ = (PCB*)PCB::running;
			UNLOCK
			dispatch();
		} else {
			UNLOCK
		}
	} else {
		UNLOCK
	}
}

void KernelEv::signal() {
	LOCK
	if (++val_ <= 0) {
		if (blocked_->get_state() != PCB::Terminated) {
			blocked_->set_state(PCB::Ready);
			Scheduler::put(blocked_);
			blocked_ = nullptr;
			UNLOCK
			dispatch();	// To make it more responsive
		} else {
			UNLOCK
		}
	} else {
		UNLOCK
		val_ = 1;
	}
}
