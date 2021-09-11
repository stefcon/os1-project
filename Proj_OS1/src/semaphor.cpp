#include "semaphor.h"
#include "krnlsem.h"
#include "lock.h"
#include <thread.h>
#include <stdio.h>


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
	HARD_LOCK
	if (val() > 0) {
		printf("WAIT ID: %d, NOT BLOCKED\n", Thread::getRunningId());
	}
	else {
		printf("WAIT ID: %d, BLOCKED\n", Thread::getRunningId());
	}
	HARD_UNLOCK
	return my_impl_->wait(max_time_to_wait);
}

void Semaphore::signal() {
	HARD_LOCK
	printf("SIGNAL ID: %d\n", Thread::getRunningId());
	HARD_UNLOCK
	if (priority_on) {
		my_impl_->priority_signal();
	}
	else {
		my_impl_->signal();
	}
}

int Semaphore::val() const {
	return my_impl_->val();
}

void Semaphore::turnOnPriorities() {
	LOCK
	priority_on = true;
	UNLOCK
}
