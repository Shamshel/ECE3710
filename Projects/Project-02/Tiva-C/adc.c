#include "adc.h"
#include "gpio.h"
#include "util.h"

#define GPIOAMSEL 0x528
#define ADCSSMUX0 0x040
#define ADCSSCTL0	0x044
#define ADCACTSS  0x000
#define ADCPSSI		0x028
#define ADCSSFIFO0 0x048
#define ADCRIS		0x004
#define ADCISC		0x00C

#define RCGCADC (int*)0x400FE638

void enableClock()
{
	*RCGCADC = 0x3;
	
}

void enableAnalogFunction(int* GPIO, unsigned char pin)
{
	gpio_alternateFunction(GPIO, pin);
	gpio_digitalEnable(GPIO, !pin);
	GPIO[GPIOAMSEL/4] = pin;
	
	
}

void adc_initADCSS0(int* ADC_base, int* GPIO, unsigned char pin, unsigned char analogIn)
{
	enableClock();
	
	enableAnalogFunction(GPIO, pin);
	
	ADC_base[ADCSSMUX0/4] = analogIn;
	
	ADC_base[ADCSSCTL0/4] = 0x02;
	
	ADC_base[ADCACTSS/4] = 0x01;
	
}

void adc_beginConversion(int* ADC_base)
{
		ADC_base[ADCPSSI/4] = 0x01;
	
}

unsigned char adc_busy(int* ADC_base)
{
	return ADC_base[ADCRIS/4];
	
}

void adc_clearBusy(int* ADC_base)
{
	ADC_base[ADCISC/4] = 0x01;
	
}

unsigned int adc_getVal(int* ADC_base)
{
	return ADC_base[ADCSSFIFO0/4];
	
}
