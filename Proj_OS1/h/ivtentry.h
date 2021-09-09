#ifndef IVTENTRY_H_
#define IVTENTRY_H_

#include "timer.h"
#include "list.h"

typedef unsigned char IVTNo;
class KernelEv;


class IVTEntry {
public:
	IVTEntry(IVTNo ivt_no, pInterrupt new_interrupt_routine);
	~IVTEntry();

	void signal();
	void callOldRoutine();
	void set_event(KernelEv* kernel_ev, int priority);
	void remove_event(KernelEv* kernel_ev);

	static IVTEntry* ivt_entry_table_[256];

	friend class KernelEv;

private:
	struct EventNode {
		KernelEv* kernel_ev;
		int priority;

		EventNode(KernelEv* ev, int pr) : kernel_ev(ev), priority(pr) {}
	};

	IVTNo ivt_no_;
	KernelEv* kernel_ev_;
	pInterrupt old_interrupt_routine_, new_interrupt_routine_;
	int event_num_;
	List<EventNode*> events;

	void insert_sorted(EventNode* node);
};



#endif /* IVTENTRY_H_ */
