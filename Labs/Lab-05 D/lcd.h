#ifndef LCD_H
#define LCD_H

extern void lcd_init(void);
extern void lcd_clear(short int _color);
extern void lcd_fractal(void);

extern void lcd_drawBox(int _x1, int _y1, int _w, int _h, short int _color);
extern void lcd_drawCircle(int _x1, int _y1, int _x2, int _y2, short int _color);

extern void lcd_drawString(int _x, int _y, char* string, int len);
extern void lcd_drawChar(int _x, int _y, char character, char _color);


//extern void writePixel(int _x, int _y, int _color);

#endif
