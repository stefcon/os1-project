#ifndef IVTENTRY_H_
#define IVTENTRY_H_

#include "timer.h"

typedef unsigned char IVTNo;
class KernelEv;


class IVTEntry {
public:
	IVTEntry(IVTNo ivt_no, pInterrupt new_interrupt_routine);
	~IVTEntry();

	void signal();
	void callOldRoutine();
	void set_event(KernelEv* kernel_ev);
	void remove_event();

	static IVTEntry* ivt_entry_table_[256];

	friend class KernelEv;

private:
	IVTNo ivt_no_;
	KernelEv* kernel_ev_;
	pInterrupt old_interrupt_routine_, new_interrupt_routine_;
};



#endif /* IVTENTRY_H_ */
