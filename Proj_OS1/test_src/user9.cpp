/**
 * USER9.CPP
 *
 * Creates as many child threads as possible and then exits them all.
 * Child threads are never actually removed from memory, only terminated.
 */
#include <lock.h>
#include <utils.h>
#include <thread.h>

  void dumbSleep(int delay) {
      for (int i = 0; i < 1000; ++i) {
          for (int j = 0; j < delay; ++j);
      }
  }

//class ForkThread : public Thread {
//    public:
//        ForkThread() : Thread(1024, 1) {}
//        virtual void run();
//        virtual Thread* clone() const {
//            return new ForkThread();
//        }
//        ~ForkThread() {
//            waitToComplete();
//        }
//        static volatile int failedFork;
//};
//
//volatile int ForkThread::failedFork = false;
//
//void ForkThread::run() {
//    while (!failedFork) {
//        ID forked = fork();
//        if (forked < 0) {
//            syncPrintf("Failed to fork in thread %d!\n", getRunningId(), lock_counter);
//            failedFork = true;
//            break;
//        } else if (forked == 0) {
//            syncPrintf("We are in child %d\n", getRunningId());
//        } else {
//            syncPrintf("Cloned thread: %d\n", forked, lock_counter);
//            dumbSleep(10000);
//        }
//    }
//    waitForForkChildren();
//}
//
////void tick() {}
//
//int userMain (int argc, char* argv[]) {
//    (void) argc;
//    (void) argv;
//    ForkThread t;
//    t.start();
//    return 0;
//}
