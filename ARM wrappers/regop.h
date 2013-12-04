//regop.h

#ifndef REGOP_H
#define REGOP_H

void writeReg(unsigned char* baseAddr, unsigned int offset, unsigned int regVal);
unsigned int readReg(unsigned char* baseAddr, unsigned int offset);
void andWriteReg(unsigned char* baseAddr, unsigned int offset, unsigned int regVal);
void orWriteReg(unsigned char* baseAddr, unsigned int offset, unsigned int regVal);

#endif
