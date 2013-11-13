#include "LCD.H"
#include "GPIO.h"

//basic LCD initialization
void lcdInit(void)
{
	//It may be a good idea to reset the LCD and turn on the backlight here (can be done using GPIO)
	//LCD reset
	//backlight
	
	//see the datasheet for an explanation of the following
	//  (shouldn't need to change anything below)
	writeReg(0x0000,0x0001);    delayMS(50);   /* Enable LCD Oscillator */
	writeReg(0x0003,0xA8A4);    delayMS(50);   // Power control(1)
	writeReg(0x000C,0x0000);    delayMS(50);   // Power control(2)
	writeReg(0x000D,0x080C);    delayMS(50);   // Power control(3)
	writeReg(0x000E,0x2B00);    delayMS(50);   // Power control(4)
	writeReg(0x001E,0x00B0);    delayMS(50);   // Power control(5)
	writeReg(0x0001,0x2B3F);    delayMS(50);   // Driver Output Control /* 320*240 0x2B3F */
	writeReg(0x0002,0x0600);    delayMS(50);   // LCD Drive AC Control
	writeReg(0x0010,0x0000);    delayMS(50);   // Sleep Mode off
	writeReg(0x0011,0x6070);    delayMS(50);   // Entry Mode
	writeReg(0x0005,0x0000);    delayMS(50);   // Compare register(1)
	writeReg(0x0006,0x0000);    delayMS(50);   // Compare register(2)
	writeReg(0x0016,0xEF1C);    delayMS(50);   // Horizontal Porch
	writeReg(0x0017,0x0003);    delayMS(50);   // Vertical Porch
	writeReg(0x0007,0x0133);    delayMS(50);   // Display Control
	writeReg(0x000B,0x0000);    delayMS(50);   // Frame Cycle control
	writeReg(0x000F,0x0000);    delayMS(50);   // Gate scan start position
	writeReg(0x0041,0x0000);    delayMS(50);   // Vertical scroll control(1)
	writeReg(0x0042,0x0000);    delayMS(50);   // Vertical scroll control(2)
	writeReg(0x0048,0x0000);    delayMS(50);   // First window start
	writeReg(0x0049,0x013F);    delayMS(50);   // First window end
	writeReg(0x004A,0x0000);    delayMS(50);   // Second window start
	writeReg(0x004B,0x0000);    delayMS(50);   // Second window end
	writeReg(0x0044,0xEF00);    delayMS(50);   // Horizontal RAM address position
	writeReg(0x0045,0x0000);    delayMS(50);   // Vertical RAM address start position
	writeReg(0x0046,0x013F);    delayMS(50);   // Vertical RAM address end position
	writeReg(0x0030,0x0707);    delayMS(50);   // gamma control(1)
	writeReg(0x0031,0x0204);    delayMS(50);   // gamma control(2)
	writeReg(0x0032,0x0204);    delayMS(50);   // gamma control(3)
	writeReg(0x0033,0x0502);    delayMS(50);   // gamma control(4)
	writeReg(0x0034,0x0507);    delayMS(50);   // gamma control(5)
	writeReg(0x0035,0x0204);    delayMS(50);   // gamma control(6)
	writeReg(0x0036,0x0204);    delayMS(50);   // gamma control(7)
	writeReg(0x0037,0x0502);    delayMS(50);   // gamma control(8)
	writeReg(0x003A,0x0302);    delayMS(50);   // gamma control(9)
	writeReg(0x003B,0x0302);    delayMS(50);   // gamma control(10)
	writeReg(0x0023,0x0000);    delayMS(50);   // RAM write data mask(1)
	writeReg(0x0024,0x0000);    delayMS(50);   // RAM write data mask(2)
	writeReg(0x0025,0x8000);    delayMS(50);   // Frame Frequency
	writeReg(0x004f,0);                         // Set GDDRAM Y address counter
	writeReg(0x004e,0);                         // Set GDDRAM X address counter
	
	delayMS(50);
}

// functions to init LCD and GPIO
void lcdPortConfig(void)
{
	
 /********************************
	* GPIO setup:
	*
	*	PB0:7-D0:7
	*	PA2-DEN
	*	PA3-DDIR
	* PA4-RD
	* PA5-WR
	* PA6-CS
	* PA7-BLE
	*
	********************************/
	
	// init ports A and B all outputs
	initGPIO(GPIOA, 0x00, 0xFF);
	initGPIO(GPIOB, 0x00, 0xFF);
	
}

void writeLCD(unsigned short cmd)
{
	//latch lower byte
	//LE=1
	*LCD_DLE = 1;
	//D[7:0]=IB[7:0]
	GPIOB[GPIODATA] = cmd;
	//LE=0
	*LCD_DLE = 0;
	//D[7:0]=IB[15:8]
	GPIOB[GPIODATA] = cmd>>8;
	//DIR(DDIR) = 1
	*LCD_DDIR = 1;
	//OE(DEN) = 0
	*LCD_DEN = 0;	
	
}

// functions to send cmd/data to LCD
void writeCmd(unsigned short cmd)
{
	
	
}

void writeDat(unsigned short dat)
{
	
	
}

void writeReg(unsigned short cmd,unsigned short dat)
{
	
	
}

// helper functions
void clearLCD(unsigned short rgb)
{
	
	
}

void setCursor(unsigned short x,unsigned short y)
{
	
	
}

void setCursorInd(unsigned int ind)
{
	
	
}

void delayMS(char itime)
{
	char i = 0;
	//1ms = 0.001s
	//delay = itime*0.001*instructions/second
	//Cortex devices have 1 instruction/second arch
	//the for loop takes 5 instructions per loop
	//Change define of FREQUENCY above for varing clock
	//values.
	//TODO: (but not with the 1968) read sysctl RCC/RCC2
	//vals to determine the frequency.
	unsigned int delay = (itime*(0.001*FREQUENCY))/5;
	
	for(;i<delay; i++); //"I CAN PLAY THE SIT-'N-SPIN!"
	
}

// functions to draw boxes
void makeBox(unsigned short x,unsigned short y,unsigned short rgb)
{
	
	
}

void makeOpenBox(unsigned short x,unsigned short y,unsigned short rgb)
{
	
	
}
