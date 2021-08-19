#ifndef TIMER_H_
#define TIMER_H_

extern volatile unsigned context_switch_wanted;
extern volatile unsigned time_left;
extern volatile unsigned time_ran_out;

typedef void interrupt (*pInterrupt) (...);

#define timerEntry 0x08

void initializeTimer();
void restoreTimer();

void tick();

void interrupt timer(...);

#endif /* TIMER_H_ */
