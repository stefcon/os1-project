#include <dos.h>
#include "SCHEDULE.H"
#include "timer.h"
#include "pcb.h"
#include "lock.h"
#include "system.h"
#include "krnlsem.h"

volatile unsigned context_switch_wanted = false;
volatile unsigned time_left = 0;
volatile unsigned time_ran_out = false;

static pInterrupt old_timer_routine;

static unsigned temporary_sp;
static unsigned temporary_ss;
static unsigned temporary_bp;

// Modification
List<PCB*> parents;

void initializeTimer() {
	#ifndef BCC_BLOCK_IGNORE
	HARD_LOCK
	old_timer_routine = getvect(timerEntry);
	setvect(timerEntry, timer);
	HARD_UNLOCK
	#endif
}

void restoreTimer() {
#ifndef BCC_BLOCK_IGNORE
	HARD_LOCK
	setvect(timerEntry, old_timer_routine);
	HARD_UNLOCK
#endif
}

void interrupt timer(...) {

	if (context_switch_wanted == false) {
		old_timer_routine();
		if (PCB::running->time_slice_ && time_left)
			--time_left;
		tick();
		KernelSem::tickAllSemaphores();
	}

	if(PCB::running->time_slice_ && !time_left) time_ran_out = true;

	if (context_switch_wanted == true || time_ran_out == true ) {
		if (lock_counter == 0) {
			context_switch_wanted = false;
			time_ran_out = false;

		#ifndef BCC_BLOCK_IGNORE
			asm {
				mov temporary_sp, sp
				mov temporary_ss, ss
				mov temporary_bp, bp
			}
		#endif

			PCB::running->sp_ = temporary_sp;
			PCB::running->ss_ = temporary_ss;
			PCB::running->bp_ = temporary_bp;

			if (PCB::running->state_ != PCB::Suspended
					&& PCB::running->state_ != PCB::Terminated
					&& PCB::running->my_id_ != System::idle_pcb_->my_id_) {
				PCB::running->state_ = PCB::Ready;
				Scheduler::put((PCB*)PCB::running);
			}

			// Modification
			if (PCB::running->new_child != nullptr) {
				PCB* old_running = (PCB*)PCB::running;
				PCB::running = old_running->new_child;
				old_running->new_child = nullptr;
				PCB::running->state_ = PCB::Ready;
			} else {
				PCB::running = Scheduler::get();

				if (PCB::running == nullptr) {
					PCB::running = System::idle_pcb_;
				}
				else if (PCB::running->children_list_.empty() == false) {
					parents.push_back((PCB*)PCB::running);
					bool has_children = true;

					while ((PCB::running = Scheduler::get()) != 0 && has_children) {
						if (PCB::running->children_list_.empty() == false) {
							parents.push_back((PCB*)PCB::running);
						}
						else {
							has_children = false;
						}
					}

					if (has_children) {
						PCB::running = parents.front();
						parents.pop_front();
					}
					while (parents.empty() == false) {
						Scheduler::put(parents.front());
						parents.pop_front();
					}
				}
			}


			PCB::running->state_ = PCB::Running;
			temporary_sp = PCB::running->sp_;
			temporary_ss = PCB::running->ss_;
			temporary_bp = PCB::running->bp_;
			time_left = PCB::running->time_slice_;


		#ifndef BCC_BLOCK_IGNORE
			asm {
				mov sp, temporary_sp
				mov ss, temporary_ss
				mov bp, temporary_bp
			}
		#endif
		}
	}
}


