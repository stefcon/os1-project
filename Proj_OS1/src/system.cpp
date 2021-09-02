#include "system.h"
#include "pcb.h"
#include "idlethr.h"
#include "userthr.h"
#include "timer.h"
#include "utils.h"

volatile PCB* System::idle_pcb_ = nullptr;
volatile PCB* System::main_pcb_ = nullptr;


void System::initializeSystem() {
	Thread* idle_thread = new IdleThread();
	System::idle_pcb_ = PCB::get_pcb_by_id(idle_thread->getId());
	System::idle_pcb_->state_ = PCB::Ready;

	System::main_pcb_ = new PCB(0, defaultTimeSlice, nullptr);
	System::main_pcb_->state_ = PCB::Running;
	PCB::running = System::main_pcb_;

	initializeTimer();
}

void System::restoreSystem() {
	restoreTimer();

	Thread* idle_thread = Thread::getThreadById(System::idle_pcb_->my_id_);
	delete idle_thread;
	System::idle_pcb_ = nullptr;

	delete System::main_pcb_;
	System::main_pcb_ = nullptr;
}




