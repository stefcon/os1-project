#include "event.h"
#include "krnlev.h"
#include "lock.h"
#include "utils.h"
#include "thread.h"
#include <iostream.h>

Event::Event(IVTNo ivt_no, int priority) {
	LOCK
	my_impl_ = new KernelEv(ivt_no, priority);
	UNLOCK
}

Event::~Event() {
	HARD_LOCK
	delete my_impl_;
	my_impl_ = nullptr;
	HARD_UNLOCK
}

void Event::wait() {
	if (my_impl_) {
		LOCK
		cout << "\nBLOCKED - THREAD ID = " << Thread::getRunningId();
		UNLOCK
		my_impl_->wait();
	}
}

void Event::signal() {
	if (my_impl_)
		my_impl_->signal();
}
