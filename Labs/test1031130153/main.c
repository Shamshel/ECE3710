#include "gpio.h"

#include "util.h"
#include "lcd.h"
#include "ssi.h"

enum LEDColor { RED, GREEN, BLUE };

unsigned short touchScreen_getPos(int* _ssi, unsigned char cmd);
unsigned short getTouchscreenData(int* _ssi);
void waitForRelease(void);
void waitForPress(void);

int main()
{
	int i = 0;
	unsigned int Xraw = 0;
	unsigned int X = 0;
	unsigned int Yraw = 0;
	unsigned int Y = 0;
	unsigned int tmp = 0;
	unsigned char red = 0;
	unsigned char green = 0;
	unsigned char yellow = 0;
	
	gpio_enableClock(GPIO_A_CLK | GPIO_B_CLK | GPIO_D_CLK | GPIO_E_CLK | GPIO_F_CLK);
	
	ssi_enableClock(SSI1_RCGC);
	
	//gpio_digitalEnable(GPIO_F, 0x0E);
	//gpio_digitalOutput(GPIO_F, 0x0E);
	//gpio_digitalWrite(GPIO_F, 0x0E, 0x0E);
	
	//gpio_digitalEnable(GPIO_F, GPIO_PIN_4);
	//gpio_digitalInput(GPIO_F, GPIO_PIN_4);
	//gpio_pullUp(GPIO_D, GPIO_PIN_7);
	
	lcd_init();
	
	lcd_clear(0x0000);
	
	lcd_drawBox(60, 10, 80, 80, 0xF800);
	lcd_drawBox(60, 120, 80, 80, 0xFFE0);
	lcd_drawBox(60, 240, 80, 80, 0x07E0);
	
	//red
	lcd_drawBox(80, 30, 40, 40, 0x0000);
	
	//yellow
	lcd_drawBox(80, 140, 40, 40, 0x0000);
	
	//green
	lcd_drawBox(80, 260, 40, 40, 0x0000);
	
	//attach SSI1
	//PF2(SSI1Clk)->TSCK
	//PF3(SSI1Fss)->TCS
	//PD2(SSI1Rx)->TDO
	//PD3(SSI1Tx)->TDI
	
	//enable AF on pins PD0:3
	gpio_digitalEnable(GPIO_F, 0x0C);
	gpio_alternateFunction(GPIO_F, 0x0C);
	gpio_PCTL(GPIO_F, 0x2200);
	gpio_digitalEnable(GPIO_D, 0x0C);
	gpio_alternateFunction(GPIO_D, 0x0C);
	gpio_PCTL(GPIO_D, 0x2200);
	gpio_digitalInput(GPIO_A, 0x80);
	gpio_digitalEnable(GPIO_A, 0x80);
	
	//enable 
	gpio_digitalOutput(GPIO_D, 0xC2);
	gpio_digitalEnable(GPIO_D, 0xC2);
	
	
	//Config SSI for master operation, 1MHz, Microwire framing
	ssi_config(SSI1, 0x072F, 0x02, 0x00, 0x02);	
	
	while(1)
	{
		Xraw = 0;
		Yraw = 0;
		
		waitForPress();
		
		for(i = 0; i < 10000; i++)
		{
			//measure X position
			tmp = touchScreen_getPos(SSI1, 0xD8);
			if(tmp != 0xFF)
			{
				Xraw+= tmp;
			
			}
		
			//measure Y position
			Yraw += touchScreen_getPos(SSI1, 0x98);
			
		}
		
		X = Xraw/10000;
		
		Y = Yraw/10000;
		
		//red box
		if((X>0x80 && X<0x99))
		{
			red = (~red) & 0x01;
			
			lcd_drawBox(60, 10, 80, 80, 0xF800);
			
			gpio_digitalWrite(GPIO_A, GPIO_PIN_4, 0x00);
			
			if(!red)
			{
				//red
				lcd_drawBox(80, 30, 40, 40, 0x0000);
				
				gpio_digitalWrite(GPIO_A, GPIO_PIN_4, 0xFF);
				
			}
			
			waitForRelease();
			
		}
		
		//yellow box
		if((X>0x6A && X<0x7F))
		{
			yellow = (~yellow) & 0x01;
			
			lcd_drawBox(60, 120, 80, 80, 0xFFE0);
			
			gpio_digitalWrite(GPIO_A, GPIO_PIN_5, 0x00);
			
			if(!yellow)
			{
				//yellow
				lcd_drawBox(80, 140, 40, 40, 0x0000);
				
				gpio_digitalWrite(GPIO_A, GPIO_PIN_5, 0xFF);
				
			}
			
			waitForRelease();
			
		}
		
		//green box
		if((X>0x14 && X<0x4C))
		{
			green = (~green) & 0x01;
			
			lcd_drawBox(60, 240, 80, 80, 0x07E0);
			
			gpio_digitalWrite(GPIO_A, GPIO_PIN_6,0x00);
			
			if(!green)
			{
				//green
				lcd_drawBox(80, 260, 40, 40, 0x0000);
				
				gpio_digitalWrite(GPIO_A, GPIO_PIN_6, 0xFF);
				
			}

			
			waitForRelease();
			
		}

	}
	
}


unsigned short touchScreen_getPos(int* _ssi, unsigned char cmd)
{
	unsigned short tmp = 0;
	
	//command format
	//S must always be 1
	//A2:0=101 measures X position
	//A2:0=001 measures Y position
	//MODE=1 performs 8-bit conversion, MODE=0 performs 12 bit conversion
	//SER/DFR single-ended/differential reference select
	//PD1:0
	
	tmp = ssi_transfer(SSI1, cmd);
	
	return tmp>>8;
	
}

void waitForRelease()
{
	unsigned int tirq = 0;
	int i = 0;
	
	while(tirq < 950)
			{
				tirq = 0;
				
				for(i = 0; i < 1000; i++)
				{
					tirq += gpio_digitalRead(GPIO_A, GPIO_PIN_7) >> 7;
			
				}
				
			}
	
}
void waitForPress()
{
	unsigned int tirq = 1000;
	int i = 0;
	
	while(tirq > 50)
	{
		tirq = 0;
			for(i = 0; i < 1000; i++)
			{
				tirq += gpio_digitalRead(GPIO_A, GPIO_PIN_7) >> 7;
				
				//util_delayMS(1);
			}
		}
}

