#include "ssi.h"
#include "util.h"

void ssi_enableClock(int _mask)
{
	util_writeMask((int*)RCGCSSI, _mask, 0xFF);
	
}

void ssi_config(int* _ssi, unsigned int _config_0, unsigned int _config_1, unsigned int _config_clock, unsigned int _config_prescale)
{
		//assume ports and clocks have been enabled and configured
	
	//disable SSI
	util_writeMask((_ssi+SSICR1/4), 0xFF, 0x00);
	
	//config clock
	util_writeMask((_ssi+SSICC/4), 0xFF, _config_clock);
	
	//set SSICR0
	util_writeMask((_ssi+SSICR0/4), 0xFFFF, _config_0);	
	
	//set prescaler
	//BR=SysClk/(CPSDVSR*(1+SCR))
	//CPSDVSR=SysClk/(BR*(1+SCR))
	util_writeMask((_ssi+SSICPSR/4), 0xFF, _config_prescale);
	
	//enable SSI/write config 1
	util_writeMask((_ssi+SSICR1/4), 0xFF, _config_1|0x02);
	
}

unsigned short ssi_transfer(int* _ssi, unsigned short _data)
{
	util_writeMask((_ssi+SSIDR/4), 0xFFFF, _data);
	
	//wait while SSI busy
	while(util_readMask(_ssi+SSISR/4, 0x10) != 0x10);
	
	//return retrieved data
	return (unsigned short)util_readMask(_ssi+SSIDR/4, 0xFFFF);
	
}
