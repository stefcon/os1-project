#include "semaphor.h"
#include "krnlsem.h"
#include "lock.h"


Semaphore::Semaphore(int init) {
	LOCK
	my_impl_ = new KernelSem(init);
	UNLOCK
}

Semaphore::~Semaphore() {
	LOCK
	delete my_impl_;
	my_impl_ = nullptr;
	UNLOCK
}

int Semaphore::wait(Time max_time_to_wait) {
	return my_impl_->wait(max_time_to_wait);
}

void Semaphore::signal() {
	my_impl_->signal();
}

int Semaphore::val() const {
	return my_impl_->val();
}
