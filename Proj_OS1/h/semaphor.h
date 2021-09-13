#ifndef SEMAPHOR_H_
#define SEMAPHOR_H_

#include <thread.h>

typedef unsigned int Time;
class KernelSem;

class Semaphore {
public:
	Semaphore (int init = 1);
	virtual ~Semaphore ();

	virtual int wait (Time max_time_to_wait);
	virtual void signal();

	// Modif
	void pair(Thread* t1, Thread* t2);

	int val () const;	// Returns the current value of the semaphore

private:
	KernelSem* my_impl_;
	friend class PCB;
};

#endif /* SEMAPHOR_H_ */
