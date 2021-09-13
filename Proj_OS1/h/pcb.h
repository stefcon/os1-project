#ifndef PCB_H_
#define PCB_H_

#include "list.h"
#include <thread.h>
#include "semaphor.h"
#include "utils.h"


const StackSize max_stack_size = 65535;
extern volatile PCB* fork_child;
extern volatile PCB* fork_parent;


class PCB {
public:
	enum State { Initialized, Ready, Suspended, Running, Terminated };

	PCB(StackSize stack_size, Time time_slice, Thread* my_thread = nullptr);
	~PCB();

	void start();
	void waitToComplete();
	ID get_id() const volatile;
	static ID get_running_id();
	static Thread * get_thread_by_id(ID id);
	void set_state(State new_state) volatile;
	State get_state() const volatile;
	unsigned get_time_slice() const volatile;

	static void wrapper();


	static List<PCB*> all_pcbs_;
	static volatile PCB* running;

	friend void interrupt timer(...);
	friend class System;
	friend class Thread;
	friend class KernelSem;

	// Modif
	static void pair(Semaphore* sem, Thread* t1, Thread* t2);

	struct PairInfo {
		PCB* pcb1;
		PCB* pcb2;
		KernelSem* sem;
		bool pair_blocked;
		bool pair_critical;
		bool sem_blocked;
		PairInfo(PCB* p1, PCB* p2, KernelSem* s) : pcb1(p1), pcb2(p2), sem(s),
				pair_blocked(false), pair_critical(false), sem_blocked(false) {}
	};

private:
	unsigned sp_, ss_, bp_;
	unsigned* stack_;
	StackSize stack_size_;
	unsigned time_slice_;
	ID my_id_;
	State state_;
	List<PCB*> suspended_list_;
	unsigned children_num_;
	Semaphore children_sem_;
	PCB* parent_;
	List<PCB*> children_list_;

	// Modif
	PairInfo* my_pair_;
	Thread* my_thread_;

	static ID threadID;

	void initializeStack(StackSize stack_size);
	static PCB* get_pcb_by_id(ID id);
	static void interrupt fork();
	static void waitForForkChildren();
	static void exit();


};


#endif /* PCB_H_ */
