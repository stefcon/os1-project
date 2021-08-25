#include "event.h"
#include "krnlev.h"
#include "lock.h"
#include "utils.h"

Event::Event(IVTNo ivt_no) {
	LOCK
	my_impl_ = new KernelEv(ivt_no);
	UNLOCK
}

Event::~Event() {
	LOCK
	delete my_impl_;
	UNLOCK
	my_impl_ = nullptr;
}

void Event::wait() {
	if (my_impl_)
		my_impl_->wait();
}

void Event::signal() {
	if (my_impl_)
		my_impl_->signal();
}
