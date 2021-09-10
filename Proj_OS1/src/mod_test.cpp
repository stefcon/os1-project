/*
 * mod_test.cpp
 *
 *  Created on: Sep 10, 2021
 *      Author: OS1
 */
#include <intLock.h>
#include <iostream.h>
#include <thread.h>
#include <utils.h>

void tick() {}

int userMain(int argc, char* argv[]) {
	int result = Thread::fork();
	if (result != -1) {
	    if (result == 0) {
	        intLock
	        cout << "Child!" << endl;
	        intUnlock
	        Thread::exit();
	    } else {
	        intLock
	        cout << "Parent!" << endl;
	        intUnlock
	        Thread::waitForForkChildren();
	    }
	}
	return 0;
}
