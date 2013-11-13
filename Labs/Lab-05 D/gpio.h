#ifndef GPIO_H
#define GPIO_H

#define GPIO_A		(int *)0x40004000
#define GPIO_B		(int *)0x40005000
#define GPIO_C		(int *)0x40006000
#define GPIO_D		(int *)0x40007000
#define GPIO_E		(int *)0x40024000
#define GPIO_F		(int *)0x40025000

#define GPIO_A_CLK		0x01
#define GPIO_B_CLK		0x02
#define GPIO_C_CLK		0x04
#define GPIO_D_CLK		0x08
#define GPIO_E_CLK		0x10
#define GPIO_F_CLK		0x20

#define GPIO_PIN_0		0x01
#define GPIO_PIN_1		0x02
#define GPIO_PIN_2		0x04
#define GPIO_PIN_3		0x08
#define GPIO_PIN_4		0x10
#define GPIO_PIN_5		0x20
#define GPIO_PIN_6		0x40
#define GPIO_PIN_7		0x80

#define write		((int *)

void gpio_enableClock(int _portMask);

extern void gpio_digitalEnable(int * _port, int _pinMask);
extern void gpio_digitalOutput(int * _port, int _pinMask);
extern void gpio_digitalInput(int * _port, int _pinMask);
extern void gpio_pullUp(int * _port, int _pinMask);
extern void gpio_pullDown(int * _port, int _pinMask);
extern void gpio_alternateFunction(int* _port, int _pinMask);
extern void gpio_PCTL(int* _port, int _AF);

//extern void gpio_digitalWrite(int * _port, int _pinMask, int _state);
extern int gpio_digitalRead(int * _port, int _pinMask);

#define gpio_digitalWrite(_port, _pinMask, _state) ((int *)_port)[_pinMask] = _state
#define gpio_digitalRead(_port, _pinMask) ((const int *)_port)[_pinMask]

#define gpio_digitalWriteHigh(_port, _pinMask) ((int *)_port)[_pinMask] = 0xFF
#define gpio_digitalWriteLow(_port, _pinMask) ((int *)_port)[_pinMask] = 0

#endif
