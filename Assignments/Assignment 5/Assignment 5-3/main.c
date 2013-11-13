// main.c
// Runs on LM3S1968
 
 #define FREQUENCY 12000000 //Hz

void MSDelay(unsigned int itime);

int main(void)
{
	MSDelay(1000); //delay 1 second
	
	//Fin
	while(1);
	
}


void MSDelay(unsigned int itime)
{
	unsigned int i = 0;
	//1ms = 0.001s
	//delay = itime*0.001*instructions/second
	//Cortex devices have 1 instruction/second arch
	//the for loop takes 5 instructions per loop
	//Change define of FREQUENCY above for varing clock
	//values.
	//TODO: (but not with the 1968) read sysctl RCC/RCC2
	//vals to determine the frequency.
	unsigned int delay = (itime*(0.001*FREQUENCY))/5;
	
	for(;i<delay; i++); //"I CAN PLAY THE SIT-'N-SPIN!"
	
}
