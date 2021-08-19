#include "ivtentry.h"
#include "krnlev.h"
#include "lock.h"
#include "utils.h"

IVTEntry* IVTEntry::ivt_entry_table_[256] = { nullptr };

IVTEntry::IVTEntry(IVTNo ivt_no, pInterrupt new_interrupt_routine) : ivt_no_(ivt_no),
		old_interrupt_routine_(nullptr), new_interrupt_routine_(new_interrupt_routine), kernel_ev_(nullptr) {

	IVTEntry::ivt_entry_table_[ivt_no_] = this;

}

IVTEntry::~IVTEntry() {

	IVTEntry::ivt_entry_table_[ivt_no_] = nullptr;
	kernel_ev_ = nullptr;

}

void IVTEntry::signal() {
	if (kernel_ev_)
		kernel_ev_->signal();
}

void IVTEntry::callOldRoutine() {
	old_interrupt_routine_();
}


void IVTEntry::set_event(KernelEv* kernel_ev) {
	kernel_ev_ = kernel_ev;
}



