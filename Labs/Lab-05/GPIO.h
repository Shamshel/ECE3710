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
#define GPIOICR			0x41C
#define GPIOAFSEL		0x420
#define GPIODR2R		0x500
#define GPIODR4R		0x504
#define GPIODR8R		0x508
#define GPIOODR			0x50C
#define GPIOPUR			0x510
#define GPIOPDR			0x514
#define GPIODEN			0x51C
#define GPIOLOCK		0x520
#define GPIOCR			0x524
#define GPIOPCTL		0x52C

//base addresses
#define GPIOA ((unsigned int*) 0x40004000)
#define GPIOB ((unsigned int*) 0x40005000)
#define GPIOC ((unsigned int*) 0x40006000)
#define GPIOD ((unsigned int*) 0x40007000)
#define GPIOE ((unsigned int*) 0x40024000)
#define GPIOF ((unsigned int*) 0x40025000)

void initGPIO(unsigned int* baseAddr, unsigned char inMask, unsigned char outMask);
void initGPIO_AF(unsigned int* baseAddr, unsigned char AF_Mask, unsigned int AF_PCTL);

#endif
