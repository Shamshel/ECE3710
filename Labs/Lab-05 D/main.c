#include "gpio.h"

#include "util.h"
#include "lcd.h"
#include "ssi.h"

enum LEDColor { RED, GREEN, BLUE };

int main()
{
	gpio_enableClock(GPIO_A_CLK | GPIO_B_CLK | GPIO_E_CLK);
	
	lcd_init();
	
	lcd_clear(0x0000);
	
	while(1);
	
}

