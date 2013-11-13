// main.c
// Runs on LM3S1968

 /*
	*
	* red -> PC.0, green -> PC.1, yellow -> PC.2
	*
	* 5KHz square -> PB.0
	*
	*/
#include "SYSCTL.h"
#include "GPTM.h"
#include "GPIO.h"
#include "regop.h"

#define RED_STATE 0
#define YELLOW_STATE 1
#define GREEN_STATE 2

#define RED_LIGHT 0x01
#define YELLOW_LIGHT 0x04
#define GREEN_LIGHT 0x02
	
volatile unsigned char timeoutCount = 0;
volatile unsigned char state = 0;
	
int main(void)
{	
	//enable clocking on GPIOB:C
	enableRCGC(RCGC2, 0x06);
	//enable clocking on TMR0
	enableRCGC(RCGC1, 0x10000);
	
	//enable digital input on GPIOC
	initGPIO(GPIOC, 0x07, 0x00);
	
	//enable AF on GPIOB
	initGPIO_AF(GPIOB, 0x01, 0x00);
	
	//enable 16-bit input edge count mode on timer 0/a
	//setup for 5 second interval
	// 5KHz = 5000cycles/second * 5 seconds = 25,000 cycles
	initEdgeCount(GPTM0, 25000, 0x02);
	
	//do other "useful" tasks:
	while(1);
	
	return 0;
	
}

void Timer0A_Handler(void)
{
	//clear interrupt
	writeReg(GPTM0, GPTMICR, 0x01);
	
	//increment timeoutCount
	timeoutCount++;
	
	//check current state and timeout count
	//green == 2, yellow == 1, red == 0:
	if(state == RED_STATE)
	{
		//check to see if 20 seconds have passed
		if(timeoutCount == 4)
		{
			//Red->Green
			timeoutCount = 0;
			state = GREEN_STATE;

			writeReg(GPIOB, GPIODATA, GREEN_LIGHT);
			
		}
		
	}
	
	else if(state == YELLOW_STATE)
	{
		//check to see if 5 seconds have passed
		if(timeoutCount == 1)
		{
			//Yellow->Red
			timeoutCount = 0;
			state = RED_STATE;

			writeReg(GPIOB, GPIODATA, RED_LIGHT);
			
		}
		
	}
	
	else if(state == GREEN_STATE)
	{
		//check to see if 30 seconds have passed
		if(timeoutCount == 6)
		{
			//Green->Yellow
			timeoutCount = 0;
			state = YELLOW_STATE;

			writeReg(GPIOB, GPIODATA, YELLOW_LIGHT);
			
		}
		
	}
	
	
}

