#ifndef LCD_H
#define LCD_H

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

extern void lcd_init(void);
extern void lcd_clear(short int _color);
extern void lcd_fractal(void);

extern void lcd_drawBox(int _x1, int _y1, int _w, int _h, short int _color);
extern void lcd_drawCircle(int _x1, int _y1, int _x2, int _y2, short int _color);

extern void lcd_drawString(int _x, int _y, char* string, int len, unsigned short _color, const unsigned char* _font);
extern void lcd_drawChar(int _x, int _y, char character, unsigned short _color, const unsigned char* _font);
extern char* lcd_makeString(unsigned int input, char* inArr);


//extern void writePixel(int _x, int _y, int _color);

#endif
