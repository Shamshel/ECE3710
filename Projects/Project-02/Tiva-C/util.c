#include "util.h"

void util_writeMask(int * _addr, int _mask, int _val)
{
	*_addr = (*_addr & ~_mask) | (_val & _mask);
}
int util_readMask(int * _addr, int _mask)
{
	return *_addr & _mask;
}

extern void util_delayMS(int _ms)
{
	_ms *= 16000;  //TODO: tune this
	while(_ms --);
}
