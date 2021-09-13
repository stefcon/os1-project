#ifndef THREAD_H_
#define THREAD_H_

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;

typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice = 2; // default = 2**55ms

typedef int ID;

class PCB;


class Thread {
public:

	void start();
	void waitToComplete();
	virtual ~Thread();

	ID getId();
	static ID getRunningId();
	static Thread * getThreadById(ID id);

	// Fork task
	static ID fork();
	static void exit();
	static void waitForForkChildren();

	virtual Thread* clone() const;
	// Modif
	Thread(void (*f) (void*), void* param,
			StackSize stack_size = defaultStackSize, Time time_slice = defaultTimeSlice);

protected:
	friend class PCB;
	Thread (StackSize stack_size = defaultStackSize, Time time_slice = defaultTimeSlice);
	virtual void run() {}

private:
	PCB* my_pcb_;
};

void dispatch();

#endif /* THREAD_H_ */
