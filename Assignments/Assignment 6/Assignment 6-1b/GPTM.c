//GPTM.c

#include "GPTM.h"
#include "GPIO.h"
#include "regop.h"

void initEdgeCount(unsigned char* gptmBaseAddr, unsigned int timeout, unsigned int irqMask)
{
	//disable timer
	writeReg(gptmBaseAddr, GPTMCTL, 0x00);
	
	//write GPTMCFG with 0x4
	writeReg(gptmBaseAddr, GPTMCFG, 0x04);
	
	//write the TACMR field to 0x0 and the TAMR to 0x3
	writeReg(gptmBaseAddr, GPTMTAMR, 0x03);
	
	//load the start value
	writeReg(gptmBaseAddr, GPTMTAILR, timeout);
	
	//set interrupt mask
	orWriteReg(gptmBaseAddr, GPTMIMR, irqMask);
	
	//define the event type by writing TAEVENT in GPTMCTL and enable
	//(posedge)
	writeReg(gptmBaseAddr, GPTMCTL, 0x01);
	
}

