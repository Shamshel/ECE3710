// UART.c

#include "UART.h"
#include "regop.h"

void initUART(unsigned char* baseAddr, unsigned int baud, unsigned int intMask)
{
	unsigned int FBRD = 0;
	unsigned int IBRD = 0;
	
	//disable UART module
	writeReg(baseAddr, UARTCTL, 0x00);
	
	//determine BRD:
	FBRD = FREQUENCY/(16.0*baud)*1000; //preserve 4 digits of fractional
	
	IBRD = FBRD/1000.0; //truncate decimal portion
	IBRD = IBRD*1000;
	FBRD -= IBRD; //remove integer portion from fractional
	
	IBRD = IBRD/1000; //restore integer portion
	
	FBRD = (FBRD/1000.0)*64+0.5; //determine value to be loaded
	
	//write BRD values
	writeReg(baseAddr, UARTIBRD, IBRD);
	writeReg(baseAddr, UARTFBRD, FBRD);
	
	//enable with one stop bit, no parity, FIFO's disabled, interrupts
	writeReg(baseAddr, UARTLCRH, 0x60);
	writeReg(baseAddr, UARTIM, intMask);
	
	//enable UART module Rx and Tx
	writeReg(baseAddr, UARTCTL, 0x301);
	
}

