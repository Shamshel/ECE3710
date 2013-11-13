// main.c
// Runs on LM3S1968

#include "SYSCTL.h"
#include "UART.h"
#include "GPIO.h"
#include "regop.h"

unsigned char circBuff[256];
volatile unsigned char txOffset = 0;
volatile unsigned char rxOffset = 0;

int main(void)
{
	//enable UART0:1 clocking
	enableRCGC(RCGC1, 0x03);
	
	//UART0 is connected to PA0:1 (Rx, Tx) UART1 is connected to PD2:3(Rx, Tx)
	//enable GPIOA and GPIOD
	enableRCGC(RCGC2, 0x09);
	
	//enable AF on Rx/Tx pins
	initGPIO_AF(GPIOA, 0x03, 0x00);
	initGPIO_AF(GPIOD, 0x0C, 0x00);
	
	//enable UART0:1 modules
	initUART(UART0, 28800, 0x20); //transmit only
	initUART(UART1, 57600, 0x10); //recieve only
	
}

void UART0_Handler(void)
{
	//clear interrupt
	writeReg(UART0, UARTICR, 0x20);
	
	//check if any additional data needs be sent
	if(txOffset != rxOffset)
	{
		//transmit next byte
		writeReg(UART0, UARTDR, circBuff[txOffset]);
		
		txOffset++; //will rollover at 255+1
		
	}
	
}

void UART1_Handler(void)
{
	//clear interrupt
	writeReg(UART1, UARTICR, 0x10);
	
	//retrieve data to circular buffer
	circBuff[rxOffset] = readReg(UART1, UARTDR);
	
	rxOffset++; //will rollover at 255+1
	
}
