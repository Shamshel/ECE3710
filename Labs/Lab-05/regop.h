//regop.h

#ifndef REGOP_H
#define REGOP_H

#define FREQUENCY 16000000//Hz

void writeReg(unsigned int* baseAddr, unsigned int offset, unsigned int regVal);
unsigned int readReg(unsigned int* baseAddr, unsigned int offset);
void andWriteReg(unsigned int* baseAddr, unsigned int offset, unsigned int regVal);
void orWriteReg(unsigned int* baseAddr, unsigned int offset, unsigned int regVal);
void delayMS(unsigned int MS);

#endif
