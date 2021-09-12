#ifndef KRNLSEM_H_
#define KRNLSEM_H_

#include <semaphor.h>
#include "pcb.h"
#include "list.h"

class KernelSem {
protected:
	// Modif
	enum LockType {Free, Read, Write};
public:
	struct BlockedInfo {
		PCB* pcb;
		int time_to_wait;
		volatile int& wait_return_val;
		BlockedInfo(PCB* process, int time, volatile int& val)
		: pcb(process), time_to_wait(time), wait_return_val(val) {}
	};

	// Modif
	struct LockInfo {
		PCB* pcb;
		LockType lock;
		LockInfo(PCB* process, LockType lt) : pcb(process), lock(lt) {}
	};

	KernelSem(int init = 1);
	~KernelSem();

	int wait(Time max_time_to_wait);
	void signal();

	// Modif
	void open(char);
	void close();

	int val () const;

	static List<KernelSem*> all_semaphores_;
	static void tickAllSemaphores();


protected:
	enum ListType {Unlimited, Sleep};

	void block(Time max_time_to_wait, volatile int& wait_return_val);
	void deblock(List<BlockedInfo*>::Iterator& sem_node, int wait_return_val, ListType list_type = Unlimited);

	int val_;
	unsigned tick_counter_;
	static unsigned global_tick_counter_;

private:
	List<BlockedInfo*> unlimited_blocked_list_;
	List<BlockedInfo*> sleep_blocked_list_;
	//Modif
	List<LockInfo*> lock_blocked_list_;
	LockType curr_lock_;
	int lock_val_;

	void insert_sleep_sorted(BlockedInfo* blocked_info);
	void remove_from_all_semaphores();

};


#endif /* KRNLSEM_H_ */
