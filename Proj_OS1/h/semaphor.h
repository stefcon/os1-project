#ifndef SEMAPHOR_H_
#define SEMAPHOR_H_

typedef unsigned int Time;
class KernelSem;

class Semaphore {
public:
	Semaphore (int init = 1);
	virtual ~Semaphore ();

	virtual int wait (Time max_time_to_wait);
	virtual void signal();

	int val () const;	// Returns the current value of the semaphore

private:
	KernelSem* my_impl_;
};

#endif /* SEMAPHOR_H_ */
