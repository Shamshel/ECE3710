/* function prototypes for SSD1289 LCD on Tiva C Launchpad */

// 16-bit RGB values for some common colors
#define white          0xFFFF
#define black          0x0000
#define grey           0xF7DE
#define blue           0x001F
#define red            0xF800
#define magenta        0xF81F
#define green          0x07E0
#define cyan           0x7FFF
#define yellow         0xFFE0

//change as desired clock changes
#define FREQUENCY 16000000

 /********************************
	* GPIO setup:
	*
	*	PB0:7-D0:7
	*	PA2-DEN
	*	PA3-DDIR
	* PA4-RD
	* PA5-WR
	* PA6-CS
	* PA7-DLE
	*
	********************************/

//bit-band aliasing:

//PA2 -> 0x42000000+32*0x43FC+4*2=0x42087F88
#define LCD_DEN ((unsigned char*) 0x42087F88)
//PA3 -> 0x42000000+32*0x43FC+4*3=0x42087F8C
#define LCD_DDIR ((unsigned char*) 0x42087F8C)
//PA4 -> 0x42000000+32*0x43FC+4*4=0x42087F90
#define LCD_RD ((unsigned char*) 0x42087F90)
//PA5 -> 0x42000000+32*0x43FC+4*5=0x42087F94
#define LCD_WR ((unsigned char*) 0x42087F94)
//PA6 -> 0x42000000+32*0x43FC+4*6=0x42087FA0
#define LCD_CS ((unsigned char*) 0x42087FA0)
//PA7 -> 0x42000000+32*0x43FC+4*7=0x42087FA4
#define LCD_DLE ((unsigned char*) 0x42087FA4)

// functions to init LCD and GPIO
void lcdPortConfig(void); //setup GPIO pins
void lcdInit(void); //base LCD config

// functions to send cmd/data to LCD
void writeCmd(unsigned short cmd); //command: where we write data to
void writeDat(unsigned short dat); //data: what gets written to address given by cmd
void writeReg(unsigned short cmd,unsigned short dat); //either wrapper for above or combine above to avoid function calls

// helper functions
void clearLCD(unsigned short rgb); //set entire LCD to the color rgb (useful for debugging above)
void setCursor(unsigned short x,unsigned short y); //set current pixel to x,y
void setCursorInd(unsigned int ind); //set current pixel to ind (use index to address pixel instead of x,y)
void delayMS(char ms); //generate ms milliseconds of delay (timer or loop, your choice)

// functions to draw boxes
void makeBox(unsigned short x,unsigned short y,unsigned short rgb); //create a solid box @x,y of color given by rgb
void makeOpenBox(unsigned short x,unsigned short y,unsigned short rgb); //create outline of box @x,y of color given by rgb
