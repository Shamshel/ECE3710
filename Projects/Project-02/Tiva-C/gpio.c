#include "gpio.h"

#include "util.h"

#define GPIO_DR	0x3FC
#define GPIO_ODIR 0x400
#define GPIO_PUR 0x510
#define GPIO_PDR 0x514
#define GPIO_DEN 0x51C
#define GPIO_AFSEL 0x420
#define GPIO_PCTL 0x52C

#define GPIO_CLOCK_ENABLE_ADDR 0x400FE608

void gpio_enableClock(int _portMask)
{
	util_writeMask((int *)GPIO_CLOCK_ENABLE_ADDR, _portMask, 0xFF);
}

void gpio_digitalEnable(int * _port, int _pinMask)
{
	util_writeMask((int *)(_port + GPIO_DEN/4), _pinMask, 0xFF);
}
void gpio_digitalOutput(int * _port, int _pinMask)
{
	util_writeMask((int *)(_port + GPIO_ODIR/4), _pinMask, 0xFF);
}
void gpio_digitalInput(int * _port, int _pinMask)
{
	util_writeMask((int *)(_port + GPIO_ODIR/4), _pinMask, 0x00);
}
void gpio_pullUp(int * _port, int _pinMask)
{
	util_writeMask((int *)(_port + GPIO_PUR/4), _pinMask, 0xFF);
}
void gpio_pullDown(int * _port, int _pinMask)
{
	util_writeMask((int *)(_port + GPIO_PDR/4), _pinMask, 0xFF);
}

extern void gpio_alternateFunction(int* _port, int _pinMask)
{
	util_writeMask((int*)(_port + GPIO_AFSEL/4), _pinMask, 0xFF);
}


extern void gpio_PCTL(int* _port, int _AF)
{
	util_writeMask((int*)(_port+GPIO_PCTL/4), 0xFFFFFFFF, _AF);
}

/*
void gpio_digitalWrite(int * _port, int _pinMask, int _state)
{
	_port[_pinMask & 0xFF] = _state;
}
int gpio_digitalRead(int * _port, int _pinMask)
{
	return _port[_pinMask & 0xFF];
}
*/
