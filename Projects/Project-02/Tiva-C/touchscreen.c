#include "touchscreen.h"

#include "gpio.h"

static int down;
static Point pos;

void (* onDown)(Point) = 0;
void (* onUp)(Point) = 0;

void touchscreen_init(void (* _onDown)(Point _pos), void (_onUp)(Point _pos))
{
	down = 0;
	pos.x = 0;
	pos.y = 0;
	
	onDown = _onDown;
	onUp = _onUp;
	
	
	//attach SSI1
	//PF2(SSI1Clk)->TSCK
	//PF3(SSI1Fss)->TCS
	//PD2(SSI1Rx)->TDO
	//PD3(SSI1Tx)->TDI
	//PA7()->TIRQ
	
	
	gpio_digitalInput(GPIO_A, 0x80);
	gpio_digitalEnable(GPIO_A, 0x80);
}

//command format
//S must always be 1
//A2:0=101 measures X position
//A2:0=001 measures Y position
//MODE=1 performs 8-bit conversion, MODE=0 performs 12 bit conversion
//SER/DFR single-ended/differential reference select
//PD1:0
static short int getX(void)
{
	return ssi_transfer(SSI1, 0xD8) >> 8;
}
static short int getY(void)
{
	return ssi_transfer(SSI1, 0x98) >> 8;
}


//Updates the touchscreen state by taking the a given number of samples
void touchscreen_updatePosition(int _samples)
{
	int i = 0;
	int goodReads = 0;
	int variance = 0;
	Point buf;
	Point avg;
	avg.x = 0;
	avg.y = 0;
	
	for(i = 0; i < _samples; i++)
	{
		if(!gpio_digitalRead(GPIO_A, GPIO_PIN_7))
		{
			//measure X position
			buf.x = getX();
			//measure Y position
			buf.y = getY();
			
			if(buf.x != 0xFF && buf.y != 0xFF)
			{
				goodReads ++;
				
				//Variance sum
				variance += (avg.x - buf.x)*(avg.x - buf.x) + (avg.y - buf.y)*(avg.y - buf.y);
				
				//Running average, for variance
				avg.x = (avg.x*(goodReads - 1) + buf.x)/goodReads;
				avg.y = (avg.y*(goodReads - 1) + buf.y)/goodReads;
			}
		}
	}
	
	//Now it's the variance
	variance = variance/goodReads;
	
	//All or nothing
	if(goodReads == _samples)
	{
		if(variance/goodReads < 4000)
		{
			pos.x = avg.x;
			pos.y = avg.y;
			
			//Convert voltages to pixels
			pos.x = pos.x*10/9 - 23;
			pos.y = (240 - pos.y)*3/2;
			
			//point.x = (point.x - 73)*16/7;
			if(pos.x < 0) pos.x = 0;
			if(pos.x >= 240) pos.x = 239;
			
			if(pos.y < 0) pos.y = 0;
			if(pos.y >= 320) pos.x = 319;
			
		}
	}
}
void touchscreen_updateState(int _samples, int _ms)
{
	int tirq = 0;
	int i;
	
	for(i = 0;i < _samples;i ++)
	{
		if(gpio_digitalRead(GPIO_A, GPIO_PIN_7))
		{
			tirq ++;
		}
		
		util_delayMS(_ms);
	}

	//tirq stayed high the entire time
	if(tirq == _samples)
	{
		if(!down)
		{
			down = 1;
			if(onDown)
			{
				onDown(pos);
			}
		}
	}
	//tirq stayed low the entire time
	else
	{
		if(down)
		{
			down = 0;
			if(onUp)
			{
				onUp(pos);
			}
		}
	}
}

//Returns the current position
Point touchscreen_getPosition(void)
{
	return pos;
}
//Returns the state
int touchscreen_isDown(void)
{
	return down;
}
int touchscreen_isUp(void)
{
	return !down;
}
