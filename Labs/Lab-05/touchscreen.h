#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include "util.h"
#include "ssi.h"

extern void touchscreen_init(void (* _onDown)(Point _pos), void (_onUp)(Point _pos));

//Updates the touchscreen position by taking a given number of samples
extern void touchscreen_updatePosition(int _samples);
//Updates the touchscreen state by taking a given number of samples every _ms ms
extern void touchscreen_updateState(int _samples, int _ms);

//Returns the current position
extern Point touchscreen_getPosition(void);
//Returns the state
extern int touchscreen_isDown(void);
extern int touchscreen_isUp(void);

#endif
