#ifndef LOCK_H_
#define LOCK_H_

#include "thread.h"
#include "timer.h"

extern volatile unsigned lock_counter;

#define LOCK ++lock_counter;
#define UNLOCK \
	if (--lock_counter == 0 && time_ran_out) { dispatch(); }

#define HARD_LOCK asm{ pushf; cli; }
#define HARD_UNLOCK asm popf


#endif /* LOCK_H_ */
