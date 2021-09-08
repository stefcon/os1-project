#ifndef KRNLSEM_H_
#define KRNLSEM_H_

#include <semaphor.h>
#include "pcb.h"
#include "list.h"

class KernelSem {
public:
	struct BlockedInfo {
		PCB* pcb;
		int time_to_wait;
		int& wait_return_val;
		BlockedInfo(PCB* process, int time, int& val)
		: pcb(process), time_to_wait(time), wait_return_val(val) {}
	};

	KernelSem(int init = 1);
	~KernelSem();

	int wait(Time max_time_to_wait);
	void signal();

	int val () const;

	static List<KernelSem*> all_semaphores_;
	static void tickAllSemaphores();


protected:
	enum ListType {Unlimited, Sleep};

	void block(Time max_time_to_wait, int& wait_return_val);
	void deblock(List<BlockedInfo*>::Iterator& sem_node, int wait_return_val, ListType list_type = Unlimited);

	int val_;
	unsigned tick_counter_;
	static unsigned global_tick_counter_;

private:
	List<BlockedInfo*> unlimited_blocked_list_;
	volatile List<BlockedInfo*> sleep_blocked_list_;

	void insert_sleep_sorted(BlockedInfo* blocked_info);
	void remove_from_all_semaphores();

};


#endif /* KRNLSEM_H_ */
