//SYSCTL.h

#ifndef SYSCTL_H
#define SYSCTL_H

#define SYSCTL ((unsigned int*)0x400FE000)
#define RIS 0x050
#define IMC 0x054
#define MISC 0x058
#define RCC 0x060
#define PLLCFG 0x064
#define RCC2 0x070
#define RCGC0 0x100
#define RCGC1 0x104
#define RCGC2 0x108
#define RCGCGPIO 0x608

//specific peripherial defines

//GPIO is in RCGCGPIO
#define GPIOA_CLOCK 0x01
#define GPIOB_CLOCK 0x02
#define GPIOC_CLOCK 0x04
#define GPIOD_CLOCK 0x08
#define GPIOE_CLOCK 0x10
#define GPIOF_CLOCK 0x20


void enableRCGC(unsigned int offset, unsigned int val);

#endif
