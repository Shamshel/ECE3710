/* function prototypes for SSD1289 LCD on Tiva C Launchpad */

// 16-bit RGB values for some common colors
#define WHITE          0xFFFF
#define BLACK          0x0000
#define GREY           0xF7DE
#define BLUE           0x001F
#define RED            0xF800
#define MAGENTA        0xF81F
#define GREEN          0x07E0
#define CYAN           0x7FFF
#define YELLOW         0xFFE0

 /*
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
	
//GPIOA bit-bands:
	
//0x42000000+0x43FC*32+2*4=0x42087F88
#define WR 			(unsigned int*)0x42087F88
//0x42000000+0x43FC*32+3*4=0x42087F8C
#define CS			(unsigned int*)0x42087F8C
	
//GPIOE bit-bands:

//0x42000000+0x243FC*32+0*4=0x42487F80
#define DEN			(unsigned int*)0x42487F80
//0x42000000+0x243FC*32+1*4=0x42487F84
#define DDIR		(unsigned int*)0x42487F84
//0x42000000+0x243FC*32+2*4=0x42487F88
#define DLE			(unsigned int*)0x42487F88
//0x42000000+0x243FC*32+3*4=0x42487F8C
#define RD			(unsigned int*)0x42487F8C
//0x42000000+0x243FC*32+4*4=0x42487F90
#define RS			(unsigned int*)0x42487F90
//0x42000000+0x243FC*32+5*4=0x42487F94
#define RST			(unsigned int*)0x42487F94

// functions to init LCD and GPIO
void lcdPortConfig(void); //setup GPIO pins
void lcdInit(void); //base LCD config

// functions to send cmd/data to LCD
void writeCmd(unsigned short cmd); //command: where we write data to
void writeDat(unsigned short dat); //data: what gets written to address given by cmd
void writeLCD(unsigned short data); //send bytes to LCD; to be used in Cmd or Dat
void LCD_writeReg(unsigned short cmd,unsigned short dat); //either wrapper for above or combine above to avoid function calls

// helper functions
void clearLCD(unsigned short rgb); //set entire LCD to the color rgb (useful for debugging above)
void setCursor(unsigned short x,unsigned short y); //set current pixel to x,y
void setCursorInd(unsigned int ind); //set current pixel to ind (use index to address pixel instead of x,y)

// functions to draw boxes
void makeBox(unsigned short x,unsigned short y,unsigned short rgb); //create a solid box @x,y of color given by rgb
void makeOpenBox(unsigned short x,unsigned short y,unsigned short rgb); //create outline of box @x,y of color given by rgb
