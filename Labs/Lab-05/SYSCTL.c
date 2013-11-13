//SYSCTL.c

#include "SYSCTL.h"
#include "regop.h"

void enableRCGC(unsigned int offset, unsigned int val)
{
	orWriteReg(SYSCTL, offset, val);
	
}
