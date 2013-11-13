//GPIO.c

#include "GPIO.h"
#include "regop.h"

void initGPIO(unsigned int* baseAddr, unsigned char inMask, unsigned char outMask)
{
	baseAddr[GPIODIR/4] = outMask;
	baseAddr[GPIODR2R/4] = inMask | outMask;
	baseAddr[GPIOODR/4] = outMask;
	baseAddr[GPIOPUR/4] = inMask;
	baseAddr[GPIODEN/4] = inMask | outMask;
	
}

void initGPIO_AF(unsigned int* baseAddr, unsigned char AF_Mask, unsigned int AF_PCTL)
{
	
	baseAddr[GPIOAFSEL/4] = AF_Mask;
	
	//only needed on Tiva C
	writeReg(baseAddr, GPIOPCTL, AF_PCTL);
	
}
