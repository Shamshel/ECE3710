#ifndef ADC_H
#define ADC_H

#define ADC_0_RCGC 0x01
#define ADC_1_RCGC 0x02

#define ADC0 		(int*)0x40038000
#define ADC1 		(int*)0x40039000

void adc_initADCSS0(int* ADC_base, int* GPIO, unsigned char pin, unsigned char analogIn);
void adc_beginConversion(int* ADC_base);
unsigned char adc_busy(int* ADC_base);
void adc_clearBusy(int* ADC_base);
unsigned int adc_getVal(int* ADC_base);

#endif
