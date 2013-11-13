// main.c
// Runs on LM3S1968

//sysctl offsets
#define RCGCGPIO	0x108

//gpio offsets
#define GPIODATA	0x3FC
#define GPIODIR		0x400
#define GPIOIS		0x404
#define GPIOIBE		0x408
#define GPIOIEV		0x40C
#define GPIOIM		0x410
#define GPIORIS		0x414
#define GPIOMIS		0x418
#define GPIOAFSEL	0x420
#define GPIODR2R	0x500
#define GPIODR4R	0x504
#define GPIODR8R	0x508
#define GPIOODR		0x50C
#define GPIOPUR	 	0x510
#define GPIOPDR		0x514
#define GPIODEN		0x51C
#define GPIOLOCK	0x520
#define GPIOCR		0x524
#define GPIOPCTL	0x52C

#define FREQUENCY 12000000

//global vars
unsigned char* GPIOB = (unsigned char*) 0x40005000;
unsigned char* SYSCTL = (unsigned char*) 0x400FE000;

void initGPIO(unsigned char* gpioBase, unsigned char inputMask, unsigned char outputMask);
void rs232TX(unsigned char Data, unsigned int baudRate);

int main(void)
{
	//activate clocking
	SYSCTL[RCGCGPIO] = 0x02;
	
	//no inputs, PB3 is output
	initGPIO(GPIOB, 0x00, 0x08);
	
	//driver on port PB3 gives -10 (1) when set, +10 (0) when cleared
	//system idles high (active low)
	GPIOB[GPIODATA]=0x00;
	
	//send dummy data
	rs232TX(0xAA, 9600);
	
}

void rs232TX(unsigned char Data, unsigned int baudRate)
{
	unsigned int count;
	int i, j;
	
	//according to wikipedia: 1 baud is in symbols/second or pulses/second
	//duration time is 1/baud seconds, thus every 1/baud, a new symbol should appear
	//# of cycles per second * delay = loop count
	//loop takes 5 cycles per iteration
	count = (FREQUENCY/baudRate)/5;
	
	//transmit start bit
	GPIOB[GPIODATA]=0x08;
	for(i=0;i<count;i++);
	
	for(i=0;i<8;i++)
		{
			//assign PB3 with current LSB of Data
			GPIOB[GPIODATA]=(Data%2)<<3;
			Data=Data>>1;
			for(j=0;j<count;j++);
			
		}
		
		//stop bit and idle value
		GPIOB[GPIODATA]=0x00;
		
}

void initGPIO(unsigned char* gpioBase, unsigned char inputMask, unsigned char outputMask)
{
	//disable AFSEL
	gpioBase[GPIOAFSEL] = 0x00;
	//set digital direction
	gpioBase[GPIODIR] = outputMask;
	//set 2mA pin power
	gpioBase[GPIODR2R] = inputMask | outputMask;
	//give outputs open drain
	gpioBase[GPIOODR] = outputMask;
	//enable pullup resistors on inputs
	gpioBase[GPIOPUR] = inputMask;
	//enable all used pins
	gpioBase[GPIODEN] = inputMask | outputMask;
	
}
