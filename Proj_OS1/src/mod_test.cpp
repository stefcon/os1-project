#include <stdio.h>
#include <semaphor.h>
#include <thread.h>
#include <intLock.h>
#include <iostream.h>

void tick() {}

Semaphore sem(1, 2);

int userMain(int argc, char* argv[]) {
	int pid = Thread::fork();

	if (pid == 0) {
		sem.wait(1);
		sem.wait(1);
		intLock
		printf("Value of sem: %d for id %d\n", sem.val(), Thread::getRunningId());
		intUnlock
		Thread::exit();
	}


	Thread::waitForForkChildren();
	intLock
	printf("Value of sem: %d for id %d\n", sem.val(), Thread::getRunningId());
	intUnlock

	return 0;
}



