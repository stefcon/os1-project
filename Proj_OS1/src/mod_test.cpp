#include <stdio.h>
#include <stdlib.h>
#include <intLock.h>
#include <thread.h>

void tick() {}

struct Arguments {
	int a;
	int b;
};

void sum(void * arg) {
	Arguments* args = (Arguments*) arg;
	intLock
	int c = args->a + args->b;
	printf("Sum of the two numbers is %d\n", c);
	intUnlock
	delete args;
}

void mul(void * arg) {
	Arguments* args = (Arguments*) arg;
	intLock
	int c = args->a * args->b;
	printf("Product of the two numbers is %d\n", c);
	intUnlock
}


int userMain(int argc, char* argv[]) {
	Arguments* args = new Arguments();
	args->a = 5;
	args->b = 10;
	Thread* test_thread1 = new Thread(sum, args);
	Thread* test_thread2 = new Thread(mul, args);
	delete test_thread1;
	delete test_thread2;
	delete args;
}
