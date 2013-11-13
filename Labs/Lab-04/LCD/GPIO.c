//GPIO.c

#include "GPIO.h"

void initGPIO(unsigned char* baseAddr, unsigned char inMask, unsigned char outMask)
{
	baseAddr[GPIODIR] = outMask;
	baseAddr[GPIODR2R] = inMask | outMask;
	baseAddr[GPIOODR] = outMask;
	baseAddr[GPIOPUR] = inMask;
	baseAddr[GPIODEN] = inMask | outMask;
	
}
