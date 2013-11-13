#ifndef SSI_H
#define SSI_H

//base addresses
#define SSI0 (int*)0x40008000
#define SSI1 (int*)0x40009000
#define SSI2 (int*)0x4000A000
#define SSI3 (int*)0x4000B000

//offsets
#define SSICR0		0x000
#define SSICR1		0x004
#define SSIDR			0x008
#define SSISR			0x00C
#define SSICPSR		0x010
#define SSIIM			0x014
#define SSIRIS		0x018
#define SSIMIS		0x01C
#define SSIICR		0x020
#define SSIDMACTL 0x024
#define SSICC			0xFC8

//arbitrary defines
#define SSI0_RCGC 0x01
#define SSI1_RCGC 0x02
#define SSI2_RCGC 0x03
#define SSI3_RCGC 0x04

#define RCGCSSI 0x400FE61C

void ssi_enableClock(int _mask);
void ssi_config(int* _ssi, unsigned int _config_0, unsigned int _config_1, unsigned int _config_clock, unsigned int _config_prescale);
unsigned short ssi_transfer(int* _ssi, unsigned short _data);

#endif
