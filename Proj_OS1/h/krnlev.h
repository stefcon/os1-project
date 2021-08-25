#ifndef KRNLEV_H_
#define KRNLEV_H_

#include <event.h>
#include "krnlsem.h"

class KernelEv {
public:
	KernelEv(IVTNo ivt_no);
	~KernelEv();

	void wait();
	void signal();

private:
	PCB* owner_, * blocked_;
	IVTNo ivt_no_;
	int val_;

	void releaseOwner();

};


#endif /* KRNLEV_H_ */
