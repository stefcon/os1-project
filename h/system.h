#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "pcb.h"

class System {
public:
	static volatile PCB* idle_pcb_;
	static volatile PCB* main_pcb_;
	static void initializeSystem();
	static void restoreSystem();
};


#endif /* SYSTEM_H_ */
