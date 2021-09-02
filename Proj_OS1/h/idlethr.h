#ifndef IDLETHR_H_
#define IDLETHR_H_

#include <thread.h>

class IdleThread : public Thread {
public:
	IdleThread() : Thread(1024, 1) {}
	~IdleThread() {}
	void run() { while(1); }
};


#endif /* IDLETHR_H_ */
