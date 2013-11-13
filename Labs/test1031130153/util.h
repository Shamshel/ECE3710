#ifndef UTIL_H
#define UTIL_H

#define FREQUENCY 16000000//Hz

extern void util_writeMask(int * _addr, int _mask, int _val);
extern int util_readMask(int * _addr, int _mask);

extern void util_delayMS(int _ms);

#endif
