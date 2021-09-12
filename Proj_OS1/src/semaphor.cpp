#include "semaphor.h"
#include "krnlsem.h"
#include "lock.h"


Semaphore::Semaphore(int init) {
	HARD_LOCK
	my_impl_ = new KernelSem(init);
	HARD_UNLOCK
}

Semaphore::~Semaphore() {
	HARD_LOCK
	delete my_impl_;
	my_impl_ = nullptr;
	HARD_UNLOCK
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

// Modif
void Semaphore::open(char c) {
	my_impl_->open(c);
}

void Semaphore::close() {
	my_impl_->close();
}

