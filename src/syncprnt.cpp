#include <dos.h>
#include <stdio.h>
#include <stdarg.h>
#include "lock.h"

int syncPrintf(const char *format, ...)
{
	int res;
	va_list args;
	LOCK
		va_start(args, format);
	res = vprintf(format, args);
	va_end(args);
	UNLOCK
		return res;
}
