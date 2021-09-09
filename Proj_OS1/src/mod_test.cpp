#include <stdlib.h>
#include <thread.h>
#include <semaphor.h>
#include <event.h>
#include <intLock.h>
#include <stdio.h>


PREPAREENTRY(9,1);

void tick() {}

int userMain(int argc, char* argv[]) {
    for(int i = 0; i < 50; i++){
        int pid = Thread::fork();
        if (pid) continue;

        Event keyboard(9, Thread::getRunningId());
        Semaphore waiter(0);

        while(1){
            keyboard.wait();

            intLock
            printf("\nSIGNALED - THREAD ID = %d", Thread::getRunningId());
            intUnlock

            waiter.wait(rand()%10+1);
        }
    }
    Thread::waitForForkChildren();
}
