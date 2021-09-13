/*
 * mod_test.cpp
 *
 *  Created on: Sep 13, 2021
 *      Author: OS1
 */

#include <stdio.h>
#include <stdlib.h>
#include <intLock.h>
#include <semaphor.h>
#include <iostream.h>

Semaphore sem_pair(2);
Semaphore waiter(0);

void tick() {}

int userMain(int argc, char* argv[]) {

	for (int i = 0; i < 3; ++i) {
		int pid = Thread::fork();
		if (pid) continue;

		if (Thread::getRunningId() == 5) {
			sem_pair.pair(Thread::getThreadById(5), Thread::getThreadById(4));
		}

		sem_pair.wait(0);
		intLock
		printf("Entered id: %d\n", Thread::getRunningId());
		intUnlock

		waiter.wait(3);

		intLock
		printf("Exiting id: %d\n", Thread::getRunningId());
		intUnlock
		sem_pair.signal();
		break;
	}

	Thread::waitForForkChildren();
	return 0;
}

