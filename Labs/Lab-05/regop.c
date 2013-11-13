//regop.c

#include "regop.h"

void writeReg(unsigned int* baseAddr, unsigned int offset, unsigned int regVal)
{
	baseAddr[offset/4] = regVal;
	
}

unsigned int readReg(unsigned int* baseAddr, unsigned int offset)
{
	unsigned int temp = 0;
	
	temp = baseAddr[offset/4];
	
	return temp;
	
}

void andWriteReg(unsigned int* baseAddr, unsigned int offset, unsigned int regVal)
{
	//read
	unsigned int temp = readReg(baseAddr, offset);
	
	//modify
	temp = temp & regVal;
	
	//write
	writeReg(baseAddr, offset, temp);
	
}

void orWriteReg(unsigned int* baseAddr, unsigned int offset, unsigned int regVal)
{
	//read
	int temp = readReg(baseAddr, offset);
	
	//modify
	temp = temp | regVal;

	//write
	writeReg(baseAddr, offset, temp);
	
}

void delayMS(unsigned int MS)
{
	unsigned int i = 0;
	//1ms = 0.001s
	//delay = itime*0.001*instructions/second
	//Cortex devices have 1 instruction/second arch
	//the for loop takes 5 instructions per loop
	//Change define of FREQUENCY above for varing clock
	//values.
	//TODO: (but not with the 1968) read sysctl RCC/RCC2
	//vals to determine the frequency.
	unsigned int delay = (MS*(0.001*FREQUENCY))/5;
	
	for(;i<delay; i++); //"I CAN PLAY THE SIT-'N-SPIN!"
	
}

