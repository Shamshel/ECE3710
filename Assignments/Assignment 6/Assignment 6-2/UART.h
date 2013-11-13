//UART.h

#ifndef UART_H
#define UART_H

#define FREQUENCY		12000000 //12MHz default frequency

#define UARTDR			0x000	// uart data
#define UARTRSR		 	0x004	// recieve status
#define UARTFR			0x018	// flag
#define UARTILPR		0x020	// IrDA low-power register
#define UARTIBRD		0x024	// integer baud rate divisor
#define UARTFBRD		0x028	// fractional baud divisor
#define UARTLCRH		0x02C	// line control
#define UARTCTL			0x030	// control
#define UARTIFLS		0x034	// interrupt fifo level select
#define UARTIM			0x038	// interrupt mask
#define UARTRIS			0x03C	// raw interrupt status
#define UARTMIS			0x040	// masked interrupt status
#define UARTICR			0x044	// interrupt clear
#define UARTCC			0xFC8	// clock configuration

#define UART0				((unsigned char*)0x4000C000)
#define UART1				((unsigned char*)0x4000D000)
#define UART2				((unsigned char*)0x4000E000)

void initUART(unsigned char* baseAddr, unsigned int baud, unsigned int intMask);

#endif
