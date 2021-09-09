#include "ivtentry.h"
#include "krnlev.h"
#include "lock.h"
#include "utils.h"
#include <dos.h>

IVTEntry* IVTEntry::ivt_entry_table_[256] = { nullptr };

IVTEntry::IVTEntry(IVTNo ivt_no, pInterrupt new_interrupt_routine) : ivt_no_(ivt_no),
		old_interrupt_routine_(nullptr),new_interrupt_routine_(new_interrupt_routine),
		kernel_ev_(nullptr), event_num_(0) {

	IVTEntry::ivt_entry_table_[ivt_no_] = this;

}


IVTEntry::~IVTEntry() {

	IVTEntry::ivt_entry_table_[ivt_no_] = nullptr;
	kernel_ev_ = nullptr;

}


void IVTEntry::signal() {
	List<EventNode*>::Iterator iter = events.begin();
	for (; iter != events.end(); iter++) {
		(*iter)->kernel_ev->signal();
	}
}


void IVTEntry::callOldRoutine() {
	old_interrupt_routine_();
}


void IVTEntry::set_event(KernelEv* kernel_ev, int priority) {

	#ifndef BCC_BLOCK_IGNORE
	HARD_LOCK

	if (++event_num_ == 1) {
		old_interrupt_routine_ = getvect(ivt_no_);
		setvect(ivt_no_, new_interrupt_routine_);
	}
	EventNode* new_node = new EventNode(kernel_ev, priority);
	insert_sorted(new_node);

	HARD_UNLOCK
	#endif

}


void IVTEntry::remove_event(KernelEv* kernel_ev) {

	#ifndef BCC_BLOCK_IGNORE
	HARD_LOCK

	if (--event_num_ == 0) {
		setvect(ivt_no_, old_interrupt_routine_);
	}

	List<EventNode*>::Iterator iter = events.begin();
	for (; (*iter)->kernel_ev != kernel_ev; ++iter);
	events.remove_iterator(iter);

	HARD_UNLOCK
	#endif

}


void IVTEntry::insert_sorted(EventNode* node) {

	List<EventNode*>::Iterator iter = events.begin();
	for (; iter != events.end() && (*iter)->priority > node->priority; ++iter);
	events.insert(iter, node);

}



