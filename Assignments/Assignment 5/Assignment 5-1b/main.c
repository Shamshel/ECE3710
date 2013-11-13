// main.c
// Runs on LM3S1968

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to the Arm Cortex M3",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2011
   Example 2.2, Program 2.8, Figure 2.27

 Copyright 2011 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
 
//defines
 
//sysctl offsets
#define RCC			0x060
#define RCC2		0x070
#define RCGCUART	0x104
#define RCGCGPIO	0x108
 
//gpio offsets
#define GPIODATA	0x3FC
#define GPIODIR		0x400
#define GPIOIS		0x404
#define GPIOIBE		0x408
#define GPIOIEV		0x40C
#define GPIOIM		0x410
#define GPIORIS		0x414
#define GPIOMIS		0x418
#define GPIOAFSEL	0x420
#define GPIODR2R	0x500
#define GPIODR4R	0x504
#define GPIODR8R	0x508
#define GPIOODR		0x50C
#define GPIOPUR	 	0x510
#define GPIOPDR		0x514
#define GPIODEN		0x51C
#define GPIOLOCK	0x520
#define GPIOCR		0x524
#define GPIOPCTL	0x52C

//uart offsets
#define UARTDR		0x000	// uart data
#define UARTRSR		0x004	// recieve status
#define UARTFR		0x018	// flag
#define UARTILPR	0x020	// IrDA low-power register
#define UARTIBRD	0x024	// integer baud rate divisor
#define UARTFBRD	0x028	// fractional baud divisor
#define UARTLCRH	0x02C	// line control
#define UARTCTL		0x030 // control
#define UARTIFLS	0x034	// interrupt fifo level select
#define UARTIM		0x038	// interrupt mask
#define UARTRIS		0x03C	// raw interrupt status
#define UARTMIS		0x040	// masked interrupt status
#define UARTICR		0x044	// interrupt clear
#define UARTCC		0xFC8	// clock configuration

 
 //global vars
 unsigned char* SYSCTL = (unsigned char*) 0x400FE000;
 unsigned char* GPIOA = (unsigned char *) 0x40004000;
 unsigned char* UART0 = (unsigned char *) 0x4000C000;
 unsigned int* BUFFER = (unsigned int *)	0x20000400;

//function defs
void initFOSC(void);
void initGPIO_UART(unsigned char* portBase, unsigned char portMask);
void initUART(unsigned char* uartBase, unsigned char* gpioBase, unsigned char portMask, int FOSC, int baud);
unsigned char getStream(void);
unsigned char getData(unsigned char* uartBase);
void push(int data);

int main(void)
{
	initFOSC();
	initUART(UART0, GPIOA, 0x03, 15380000, 9600);
	
	getStream();

	//Fin
	while(1);
	
}

unsigned char getStream(void)
{
	unsigned char count = 0;
	unsigned char tmp;
	int pushData = 0;
	int i = 0;
	char shiftCount = 0;
	
	//first byte determines interations
	count = getData(UART0);
	for(i=0; i < count; i++)
	{
		tmp = getData(UART0);
		pushData = pushData<<8;
		pushData += tmp;
		shiftCount++;
		
		if(shiftCount==3)
		{
			shiftCount = 0;
			push(pushData);
			pushData = 0;
			
		}
		
	}
	
	return count;
	
}

unsigned char getData(unsigned char* uartBase)
{
	unsigned char tmp = 0;
	
	while(tmp != 0x40)
	{
		tmp = uartBase[UARTFR];
		tmp = tmp & 0x40;
	
	}
	
	tmp = uartBase[UARTDR];
	
	return tmp;
	
}

void push(int data)
{
	//set data
	BUFFER[0] = data;
	//assume buffer grows UP
	BUFFER += 4;
	
}

void initFOSC(void)
{
	//setup for 15.38MHZ
	//table 5-5 says BYPASS=0, SYSDIV=0x0C, PLL
	SYSCTL[RCC] 	= 0xD1;
	SYSCTL[RCC+1] = 0x03;
	SYSCTL[RCC+2] = 0x40;
	SYSCTL[RCC+3] = 0x06;
	
	//init GPIO and UART clocks
	SYSCTL[RCGCGPIO] = 0x01;
	SYSCTL[RCGCUART] = 0x01;
	
}

void initGPIO_UART(unsigned char* portBase, unsigned char portMask)
{
	//init GPIO
	portBase[GPIODEN] = portMask;
	portBase[GPIOAFSEL] = portMask;	
	
}

void initUART(unsigned char* uartBase, unsigned char* gpioBase, unsigned char portMask, int FOSC, int baud)
{
	double BRD;
	
	//init GPIO
	initGPIO_UART(gpioBase, portMask);
	
	//init UART for 9600 baud, tx/rx, no parity
	
	//disable
	uartBase[UARTCTL] = 0x00;

	BRD = FOSC/(16.0*baud);
	//BRD = FOSC/(16*9600) = 100.16
	//IBRD = 100
	uartBase[UARTIBRD] = (int)BRD;
	//FBRD = int(0.16*2^6+0.5) = 8
	uartBase[UARTFBRD] = (int)((BRD-(int)BRD)*64+0.5);
	
	uartBase[UARTLCRH] = 0x60;
	
	uartBase[UARTCTL] 	= 0x01;
	uartBase[UARTCTL+1] = 0x03;
	
}
