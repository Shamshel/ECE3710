#include "gpio.h"
#include "util.h"
#include "lcd.h"
#include "ssi.h"
#include "NRF24L01.h"

int main()
{
	volatile unsigned int tmp = 0;
	
	gpio_enableClock(GPIO_A_CLK | GPIO_B_CLK | GPIO_E_CLK | GPIO_F_CLK);
	ssi_enableClock(SSI1_RCGC);
	
	//enable AF on pins PF0:3
	gpio_digitalEnable(GPIO_F, 0x0F);
	gpio_alternateFunction(GPIO_F, 0x0F);
	gpio_PCTL(GPIO_F, 0x2222);
	
	//set up SSI for 1MHz
	//SSIClk=SysClk / (CPSDVSR*(1+SCR))
	//let CPSDVSR = 0x2
	//16MHz / (2*(1+SCR))
	//SCR = 0x7
	
	ssi_config(SSI1, 0x07C7, 0x02, 0x00, 0x02);
	
	RF24_begin();
	
	//tmp = RF24_readReg(CONFIG);
	
	while(1);
	
}
