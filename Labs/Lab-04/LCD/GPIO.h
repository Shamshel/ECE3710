//GPIO.h
#ifndef GPIO_H
#define GPIO_H

//offsets
#define GPIODATA		0x3FC 
#define GPIODIR			0x400
#define GPIOIS			0x404
#define GPIOIBE			0x408
#define GPIOIEV			0x40C
#define GPIOIM			0x410
#define GPIORIS			0x414
#define GPIOMIS			0x418
#define GPIODR2R		0x500
#define GPIODR4R		0x504
#define GPIODR8R		0x508
#define GPIOODR			0x50C
#define GPIOPUR			0x510
#define GPIOPDR			0x514
#define GPIODEN			0x51C
#define GPIOLOCK		0x520
#define GPIOCR			0x524

//base addresses
#define GPIOA ((unsigned char*) 0x40004000)
#define GPIOB ((unsigned char*) 0x40005000)
#define GPIOC ((unsigned char*) 0x40006000)
#define GPIOD ((unsigned char*) 0x40007000)
#define GPIOE ((unsigned char*) 0x40024000)
#define GPIOF ((unsigned char*) 0x40025000)

void initGPIO(unsigned char* baseAddr, unsigned char inMask, unsigned char outMask);

#endif
