#ifndef EVENT_H_
#define EVENT_H_

#include "ivtentry.h"

#define PREPAREENTRY(ivt_no, call_old) 						\
	void interrupt interrupt##ivt_no(...);					\
															\
	IVTEntry ivt_entry##ivt_no(ivt_no, interrupt##ivt_no);	\
	void interrupt interrupt##ivt_no(...) {					\
		if (call_old)										\
			ivt_entry##ivt_no.callOldRoutine();				\
		ivt_entry##ivt_no.signal();							\
		dispatch();											\
	}

typedef unsigned char IVTNo;
class KernelEv;


class Event {
public:
	Event (IVTNo ivt_no, int priority);
	~Event ();

	void wait ();

protected:
	friend class KernelEv;
	void signal();	// can call KernelEv

private:
	KernelEv* my_impl_;
};

#endif /* EVENT_H_ */
