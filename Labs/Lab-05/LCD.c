#include "LCD.H"
#include "GPIO.h"
#include "SYSCTL.h"
#include "regop.h"

//setup GPIO pins
void lcdPortConfig(void)
{
	//Assuming the following configuration:

 /*
	*	B[0-7] - 8 bit data
	*
	*	E0 - DEN
	*	E1 - DDIR
	*	E2 - DLE
	*
	*	E3 - RD
	*	E4 - RS
	*	E5 - RST
	*
	*	A2 - WR
	*	A3 - CS
	*/
	
	//enable clocking for ports A, B, and E
	enableRCGC(RCGC2, GPIOA_CLOCK|GPIOB_CLOCK|GPIOE_CLOCK);
	
	//setup GPIOs as outputs
	initGPIO(GPIOA, 0x00, 0x0C);
	initGPIO(GPIOB, 0x00, 0xFF);
	initGPIO(GPIOE, 0x00, 0x3F);
	
	writeReg(GPIOB, GPIODATA, 0x00);
	*DEN = 0;
	*DDIR = 0;
	*DLE = 0;
	*RD = 0;
	*RS = 0;
	*RST = 1;
	*WR = 1;
	*CS = 1;
	
}

//basic LCD initialization
void lcdInit(void)
{
	//initialize GPIOs
	lcdPortConfig();
	
	//It may be a good idea to reset the LCD and turn on the backlight here (can be done using GPIO)
	
	//enable adapter
	*DEN = 0;
	
	//set data direction (won't be reading in this app)
	*DDIR = 1;
	//disable read mode
	*RD = 1;
	//disable write mode
	*WR = 1;
	
	//activate system
	*CS = 0;
	
	//LCD reset
	*RST = 0;
	
	delayMS(200);
	
	*RST = 1;
	
	//backlight
	
	
	//see the datasheet for an explanation of the following
	//  (shouldn't need to change anything below)
	LCD_writeReg(0x0000,0x0001);    delayMS(50);   /* Enable LCD Oscillator */
	LCD_writeReg(0x0003,0xA8A4);    delayMS(50);   // Power control(1)
	LCD_writeReg(0x000C,0x0000);    delayMS(50);   // Power control(2)
	LCD_writeReg(0x000D,0x080C);    delayMS(50);   // Power control(3)
	LCD_writeReg(0x000E,0x2B00);    delayMS(50);   // Power control(4)
	LCD_writeReg(0x001E,0x00B0);    delayMS(50);   // Power control(5)
	LCD_writeReg(0x0001,0x2B3F);    delayMS(50);   // Driver Output Control /* 320*240 0x2B3F */
	LCD_writeReg(0x0002,0x0600);    delayMS(50);   // LCD Drive AC Control
	LCD_writeReg(0x0010,0x0000);    delayMS(50);   // Sleep Mode off
	LCD_writeReg(0x0011,0x6070);    delayMS(50);   // Entry Mode
	LCD_writeReg(0x0005,0x0000);    delayMS(50);   // Compare register(1)
	LCD_writeReg(0x0006,0x0000);    delayMS(50);   // Compare register(2)
	LCD_writeReg(0x0016,0xEF1C);    delayMS(50);   // Horizontal Porch
	LCD_writeReg(0x0017,0x0003);    delayMS(50);   // Vertical Porch
	LCD_writeReg(0x0007,0x0133);    delayMS(50);   // Display Control
	LCD_writeReg(0x000B,0x0000);    delayMS(50);   // Frame Cycle control
	LCD_writeReg(0x000F,0x0000);    delayMS(50);   // Gate scan start position
	LCD_writeReg(0x0041,0x0000);    delayMS(50);   // Vertical scroll control(1)
	LCD_writeReg(0x0042,0x0000);    delayMS(50);   // Vertical scroll control(2)
	LCD_writeReg(0x0048,0x0000);    delayMS(50);   // First window start
	LCD_writeReg(0x0049,0x013F);    delayMS(50);   // First window end
	LCD_writeReg(0x004A,0x0000);    delayMS(50);   // Second window start
	LCD_writeReg(0x004B,0x0000);    delayMS(50);   // Second window end
	LCD_writeReg(0x0044,0xEF00);    delayMS(50);   // Horizontal RAM address position
	LCD_writeReg(0x0045,0x0000);    delayMS(50);   // Vertical RAM address start position
	LCD_writeReg(0x0046,0x013F);    delayMS(50);   // Vertical RAM address end position
	LCD_writeReg(0x0030,0x0707);    delayMS(50);   // gamma control(1)
	LCD_writeReg(0x0031,0x0204);    delayMS(50);   // gamma control(2)
	LCD_writeReg(0x0032,0x0204);    delayMS(50);   // gamma control(3)
	LCD_writeReg(0x0033,0x0502);    delayMS(50);   // gamma control(4)
	LCD_writeReg(0x0034,0x0507);    delayMS(50);   // gamma control(5)
	LCD_writeReg(0x0035,0x0204);    delayMS(50);   // gamma control(6)
	LCD_writeReg(0x0036,0x0204);    delayMS(50);   // gamma control(7)
	LCD_writeReg(0x0037,0x0502);    delayMS(50);   // gamma control(8)
	LCD_writeReg(0x003A,0x0302);    delayMS(50);   // gamma control(9)
	LCD_writeReg(0x003B,0x0302);    delayMS(50);   // gamma control(10)
	LCD_writeReg(0x0023,0x0000);    delayMS(50);   // RAM write data mask(1)
	LCD_writeReg(0x0024,0x0000);    delayMS(50);   // RAM write data mask(2)
	LCD_writeReg(0x0025,0x8000);    delayMS(50);   // Frame Frequency
	LCD_writeReg(0x004f,0);                         // Set GDDRAM Y address counter
	LCD_writeReg(0x004e,0);                         // Set GDDRAM X address counter
	
	delayMS(50);
	
	clearLCD(0x0000);
	
}

//command: where we write data to
void writeCmd(unsigned short cmd)
{
	//clear D/C to set command mode
	*RS = 0;
	
	writeLCD(cmd);
	
}

//data: what gets written to address given by cmd
void writeDat(unsigned short dat)
{
	//set D/C to set data mode
	*RS = 1;
	
	writeLCD(dat);
	
}

void writeLCD(unsigned short data)
{
	//latch store
	*DLE = 1;
	
	//put lower 8 bits on GPIOB
	writeReg(GPIOB, GPIODATA, (unsigned char)data);
	
	//latch hold
	*DLE = 0;
	
	//put upper 8 bits on GPIOB
	writeReg(GPIOB, GPIODATA, (unsigned char)(data>>8));
	
	//toggle WR to send data	
	*WR = 0;
	
	delayMS(50);
	
	*WR = 1;
	
}

void LCD_writeReg(unsigned short cmd,unsigned short dat)
{
	writeCmd(cmd);
	
	writeDat(dat);
	
}

// helper functions

//set entire LCD to the color rgb (useful for debugging above)
void clearLCD(unsigned short rgb)
{
	int i = 0;
	int j = 0;
	
	setCursor(0, 0);
	
	writeCmd(0x0022);
	
	for(i = 0; i < 320; i++) // Y loop
	{
		for(j = 0; j < 240; j++) // X loop
		{
			writeDat(rgb);
			
		}
		
	}
	
}

//set current pixel to x,y
void setCursor(unsigned short x,unsigned short y)
{
	unsigned short xAddr = x;
	unsigned short yAddr = y;
	if(xAddr > 240)
	{
		xAddr = 240;
		
	}
	
	if(yAddr > 320)
	{
		yAddr = 320;
		
	}
	
	//set GDDRAM X addr counter
	LCD_writeReg(0x004E, xAddr);
	
	//set GDDRAM Y addr counter
	LCD_writeReg(0x004F, yAddr);
	
}

//set current pixel to ind (use index to address pixel instead of x,y)
void setCursorInd(unsigned int ind)
{
	setCursor(ind%240, ind/240);
	
}


// functions to draw boxes

//create a solid box @x,y of color given by rgb
void makeBox(unsigned short x,unsigned short y,unsigned short rgb)
{
	
	
}

//create outline of box @x,y of color given by rgb
void makeOpenBox(unsigned short x,unsigned short y,unsigned short rgb)
{
	
	
}
