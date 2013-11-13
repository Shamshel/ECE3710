#include "lcd.h"
#include "Ubuntu.h"
#include "gpio.h"
#include "util.h"

//Assuming the following configuration:

 /*
	*	B[0-7] - 8 bit data
	*
	*	E0 - DEN
	*	E1 - DDIR
	*	E2 - DLE
	*
	*	E3 - RD
	*	E4 - RS
	*	E5 - RST
	*
	*	A2 - WR
	*	A3 - CS
	*/

//Enables/Disables the output of the latch
void adapter_enable()
{
	gpio_digitalWrite(GPIO_E, GPIO_PIN_0,	0x00);
}
void adapter_disable()
{
	gpio_digitalWrite(GPIO_E, GPIO_PIN_0,	0xFF);
}

//Chooses the direction of the latch
void adapter_setWriteMode()
{
	gpio_digitalWrite(GPIO_E, GPIO_PIN_1, 0xFF);	//High for forward transmission
}
void adapter_setReadMode()
{
	gpio_digitalWrite(GPIO_E, GPIO_PIN_1, 0x00);	//Low for reading
}

//Latch or unlatch
void adapter_latch()
{
	gpio_digitalWrite(GPIO_E, GPIO_PIN_2,	0x00);	//Low for latch lock
}
void adapter_unlatch()
{
	gpio_digitalWrite(GPIO_E, GPIO_PIN_2,	0xFF);	//High for latch load
}

void lcd_read()
{
	//Set ~RD low and then high, to read data
	gpio_digitalWrite(GPIO_E, GPIO_PIN_3, 0x00);
	
	gpio_digitalWrite(GPIO_E, GPIO_PIN_3, 0xFF);
}
void lcd_write()
{
	//Set ~WR low and then high, to flush data
	gpio_digitalWrite(GPIO_A, GPIO_PIN_2, 0x00);
	
	gpio_digitalWrite(GPIO_A, GPIO_PIN_2, 0xFF);
}

void lcd_setCommand()
{
	//Write RS (D/~C) low for "Command" (address)
	gpio_digitalWrite(GPIO_E, GPIO_PIN_4, 0x00);
}
void lcd_setData()
{
	//Write RS (D/~C) high for data
	gpio_digitalWrite(GPIO_E, GPIO_PIN_4, 0xFF);
}

void lcd_reset()
{
	gpio_digitalWrite(GPIO_E, GPIO_PIN_5, 0x00);
	
	util_delayMS(200);
	
	gpio_digitalWrite(GPIO_E, GPIO_PIN_5, 0xFF);
}

void lcd_select()
{
	//~CS
	gpio_digitalWrite(GPIO_A, GPIO_PIN_3, 0x00);
}
void lcd_deselect()
{
	//~CS
	gpio_digitalWrite(GPIO_A, GPIO_PIN_3, 0xFF);
}


void adapter_write16(short int _val)
{
	adapter_unlatch();
	
	gpio_digitalWrite(GPIO_B, 0xFF, _val & 0xFF); //Lower 8 bits
	
	adapter_latch();
	
	gpio_digitalWrite(GPIO_B, 0xFF, _val >> 8); //Upper 8 bits
}

void writeCommand(short int _addr)
{
	lcd_setCommand();
	
	adapter_write16(_addr);
	
	lcd_write();
}
void writeData(short int _val)
{
	lcd_setData();
	
	adapter_write16(_val);
	
	lcd_write();
}

void writeReg(short int _addr, short int _val)
{
	writeCommand(_addr);
	
	writeData(_val);
}

int mandelbrot(int _x, int _y)
{
	int i;
	int zx = 0;
	int zy = 0;
	int oldx;
	
	for(i = 0;i < 100;i ++)
	{
		oldx = zx;
		zx = (oldx*oldx)/10000 - (zy*zy)/10000 + _x;
		zy = (2*oldx*zy)/10000 + _y;
		
		if(zx*zx + zy*zy > 2*2*10000*10000)
		{
			return i;
		}
	}
	
	return 100;
}

void lcd_fractal()
{
	int x, y;
	short int color;
	int m = 0;
	
	//Cursor at 0, 0
	writeReg(0x004f, 0x0000); // Set GDDRAM Y address counter
	writeReg(0x004e, 0x0000); // Set GDDRAM X address counter
	
	writeCommand(0x0022);
	
	lcd_setData();
	for(y = 0;y < 320;y ++)
	{
		for(x = 0;x < 240;x ++)
		{
			m = mandelbrot((y - 280)*70, (x - 120)*70);
			
			color = ((100 - m)*0x1F)/100;
			
			color = color + (color << 6) + (color << 11);
			//color = ((y - 160)*(y - 160) + (x - 120)*(x - 120))/10;
			
			
			gpio_digitalWrite(GPIO_E, GPIO_PIN_2,	0xFF);	//High for latch load
			
			gpio_digitalWrite(GPIO_B, 0xFF, color); //Lower 8 bits
			
			gpio_digitalWrite(GPIO_E, GPIO_PIN_2,	0x00);	//Low for latch lock
			
			gpio_digitalWrite(GPIO_B, 0xFF, color >> 8); //Upper 8 bits
			
			lcd_write();
		}
	}
}

void lcd_clear(short int _color)
{
	int i;
	
	//Cursor at 0, 0
	writeReg(0x004f, 0x0000); // Set GDDRAM Y address counter
	writeReg(0x004e, 0x0000); // Set GDDRAM X address counter
	
	writeCommand(0x0022);
	
	lcd_setData();
	for(i = 0;i < 240*320;i ++)
	{
		gpio_digitalWrite(GPIO_E, GPIO_PIN_2,	0xFF);	//High for latch load
	
		gpio_digitalWrite(GPIO_B, 0xFF, _color); //Lower 8 bits
		
		gpio_digitalWrite(GPIO_E, GPIO_PIN_2,	0x00);	//Low for latch lock
		
		gpio_digitalWrite(GPIO_B, 0xFF, _color >> 8); //Upper 8 bits
		
		lcd_write();
	}
}

void lcd_drawBox(int _x1, int _y1, int _w, int _h, short int _color)
{
	int x, y;

	//writeReg(0x0049, _w); // First window end
	
	for(y = _y1;y < _y1 + _h;y ++)
	{
		//Cursor at 0, 0
		writeReg(0x004f, y); // Set GDDRAM Y address counter
		writeReg(0x004e, _x1); // Set GDDRAM X address counter
		
		writeCommand(0x0022);
		
		lcd_setData();
		for(x = _x1;x < _x1 + _w;x ++)
		{
			gpio_digitalWrite(GPIO_E, GPIO_PIN_2,	0xFF);	//High for latch load
			
			gpio_digitalWrite(GPIO_B, 0xFF, _color); //Lower 8 bits
			
			gpio_digitalWrite(GPIO_E, GPIO_PIN_2,	0x00);	//Low for latch lock
			
			gpio_digitalWrite(GPIO_B, 0xFF, _color >> 8); //Upper 8 bits
			
			lcd_write();
		}
	}
}
void lcd_drawCircle(int _x1, int _y1, int _x2, int _y2, short int _color)
{
}

void lcd_init(void)
{
	int x, y;
	
	gpio_digitalEnable(GPIO_B, 0xFF);
	gpio_digitalOutput(GPIO_B, 0xFF);
	gpio_pullDown(GPIO_B, 0xFF);
	
	gpio_digitalEnable(GPIO_E, 0x3F);
	gpio_digitalOutput(GPIO_E, 0x3F);
	gpio_pullDown(GPIO_E, 0x3F);
	
	gpio_digitalEnable(GPIO_A, 0xFC);
	gpio_digitalOutput(GPIO_A, 0xFC);
	gpio_pullDown(GPIO_A, 0xFC);
	
	//Initialize data for transmission
	
	adapter_enable();
	adapter_setWriteMode();
	
	//adapter_write16(0xF0F0);
	
	//Set ~RD and ~WR high
	gpio_digitalWrite(GPIO_E, GPIO_PIN_3, 0xFF); //~RD
	gpio_digitalWrite(GPIO_A, GPIO_PIN_2, 0xFF); //~WR
	
	lcd_select();
	lcd_reset();
	
	//writeReg(0xAAAA, 0xFFFF);
	
	
	//see the datasheet for an explanation of the following
	//  (shouldn't need to change anything below)
	
	writeReg(0x0000, 0x0001); // Enable LCD Oscillator
	writeReg(0x0003, 0xA8A4); // Power control(1)
	writeReg(0x000C, 0x0000); // Power control(2)
	writeReg(0x000D, 0x080C); // Power control(3)
	writeReg(0x000E, 0x2B00); // Power control(4)
	writeReg(0x001E, 0x00B0); // Power control(5)
	writeReg(0x0001, 0x2B3F); // Driver Output Control 320*240 0x2B3F
	writeReg(0x0002, 0x0600); // LCD Drive AC Control
	writeReg(0x0010, 0x0000); // Sleep Mode off
	writeReg(0x0011, 0x6070); // Entry Mode
	writeReg(0x0005, 0x0000); // Compare register(1)
	writeReg(0x0006, 0x0000); // Compare register(2)
	writeReg(0x0016, 0xEF1C); // Horizontal Porch
	writeReg(0x0017, 0x0003); // Vertical Porch
	writeReg(0x0007, 0x0133); // Display Control
	writeReg(0x000B, 0x0000); // Frame Cycle control
	writeReg(0x000F, 0x0000); // Gate scan start position
	writeReg(0x0041, 0x0000); // Vertical scroll control(1)
	writeReg(0x0042, 0x0000); // Vertical scroll control(2)
	writeReg(0x0048, 0x0000); // First window start
	writeReg(0x0049, 0x013F); // First window end
	writeReg(0x004A, 0x0000); // Second window start
	writeReg(0x004B, 0x0000); // Second window end
	writeReg(0x0044, 0xEF00); // Horizontal RAM address position
	writeReg(0x0045, 0x0000); // Vertical RAM address start position
	writeReg(0x0046, 0x013F); // Vertical RAM address end position
	
	writeReg(0x0030, 0x0707); // gamma control(1)
	writeReg(0x0031, 0x0204); // gamma control(2)
	writeReg(0x0032, 0x0204); // gamma control(3)
	writeReg(0x0033, 0x0502); // gamma control(4)
	writeReg(0x0034, 0x0507); // gamma control(5)
	writeReg(0x0035, 0x0204); // gamma control(6)
	writeReg(0x0036, 0x0204); // gamma control(7)
	writeReg(0x0037, 0x0502); // gamma control(8)
	writeReg(0x003A, 0x0302); // gamma control(9)
	writeReg(0x003B, 0x0302); // gamma control(10)
	
	writeReg(0x0023, 0x0000); // RAM write data mask(1)
	writeReg(0x0024, 0x0000); // RAM write data mask(2)
	writeReg(0x0025, 0x8000); // Frame Frequency
	
	//Cursor at 0, 0
	writeReg(0x004f, 0x0000); // Set GDDRAM Y address counter
	writeReg(0x004e, 0x0000); // Set GDDRAM X address counter
	
	lcd_clear(0x0000);
	
	for(x = 0;x < 6;x ++)
	for(y = 0;y < 8;y ++)
	{
		lcd_drawBox(x*40, y*40, 20, 20, 0xFFFF);
	}
	for(x = 0;x < 6;x ++)
	for(y = 0;y < 8;y ++)
	{
		lcd_drawBox(x*40 + 20, y*40, 20, 20, 0x001F);
	}
	for(x = 0;x < 6;x ++)
	for(y = 0;y < 8;y ++)
	{
		lcd_drawBox(x*40, y*40 + 20, 20, 20, 0xF800);
	}
	for(x = 0;x < 6;x ++)
	for(y = 0;y < 8;y ++)
	{
		lcd_drawBox(x*40 + 20, y*40 + 20, 20, 20, 0x07E0);
	}
	lcd_clear(0x0000);
}

int getChar(char character)
{
	return character - 0x20;
	
}

extern void lcd_drawChar(int _x, int _y, char character, char _color)
{
	int offset = getChar(character);
	
	int x, y;
	
	for(y = 0; y<32; y++)
	{
		//Cursor at 0, 0
		writeReg(0x004f, y); // Set GDDRAM Y address counter
		writeReg(0x004e, _x); // Set GDDRAM X address counter
		
		writeCommand(0x0022);
		
		lcd_setData();
		for(x = _x; x < _x + 24; x ++)
		{
			gpio_digitalWrite(GPIO_E, GPIO_PIN_2,	0xFF);	//High for latch load
			
			gpio_digitalWrite(GPIO_B, 0xFF, _color); //Lower 8 bits
			
			gpio_digitalWrite(GPIO_E, GPIO_PIN_2,	0x00);	//Low for latch lock
			
			gpio_digitalWrite(GPIO_B, 0xFF, _color >> 8); //Upper 8 bits
			
			lcd_write();
		}
		
	}

}

void lcd_drawString(int _x, int _y, char* string, int len)
{
	
	
}

