#include "NRF24L01.h"
#include "ssi.h"
#include "util.h"

void RF24_begin()
{
	unsigned char setup = 0;
	
	// Must allow the radio time to settle else configuration bits will not necessarily stick.
  // This is actually only required following power up but some settling time also appears to
  // be required after resets too. For full coverage, we'll always assume the worst.
  // Enabling 16b CRC is by far the most obvious case if the wrong timing is used - or skipped.
  // Technically we require 4.5ms + 14us as a worst case. We'll just call it 5ms for good measure.
  // WARNING: Delay is based on P-variant whereby non-P *may* require different timing.
	util_delayMS(5);
	
	// Set 1500uS (minimum for 32B payload in ESB@250KBPS) timeouts, to make testing a little easier
  // WARNING: If this is ever lowered, either 250KBS mode with AA is broken or maximum packet
  // sizes must never be used. See documentation for a more complete explanation.
	RF24_writeReg(SETUP_RETR, (0x4<<ARD)|(0xF<<ARC));
	
	//set rf power to minimum
	setup = RF24_readReg(RF_SETUP) ;
  setup &= ~(_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;
	
	RF24_writeReg(RF_SETUP, setup);
	
	//increase data rate to 1Mbps
	setup = RF24_readReg(RF_SETUP);
	
	setup &= ~(_BV(RF_DR_LOW) | _BV(RF_DR_HIGH));
	
	RF24_writeReg(RF_SETUP, setup);
	
	//initialize 16-bit CRC
	setup = RF24_readReg(CONFIG) & ~(_BV(CRCO) | _BV(EN_CRC));
	
	setup |= _BV(EN_CRC);
	setup |= _BV(CRCO);
	
	RF24_writeReg(CONFIG, setup);
	
	//disable dynamic payloads
	RF24_writeReg(DYNPD, 0);
	
	//reset status
	RF24_writeReg(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT));
	
	//switch to RF channel 76
	RF24_writeReg(RF_CH, 76);
	
	//flush buffers, starts in known state
	RF24_flushTx();
	RF24_flushRx();
	
}

unsigned char RF24_writeReg(unsigned char reg, unsigned int dat)
{
	unsigned char status = ssi_transfer(SSI1, W_REGISTER | reg);
	ssi_transfer(SSI1, (unsigned char)dat); //LSB first
	ssi_transfer(SSI1, (unsigned char)dat>>8);
	
	return status;
	
}

unsigned int RF24_readReg(unsigned char reg)
{
	unsigned char status = ssi_transfer(SSI1, R_REGISTER | reg);
	unsigned int res = ssi_transfer(SSI1, 0x00);
	//res += ssi_transfer(SSI1, 0x00)<<8;
	
	res += status<<8;

	return res;
	
}

void RF24_tx(unsigned char data)
{
	
	
}

unsigned char RF24_rx(void)
{
	return 0x00;
	
}

void RF24_flushRx(void)
{
	ssi_transfer(SSI1, FLUSH_RX);
	
}

void RF24_flushTx(void)
{
	ssi_transfer(SSI1, FLUSH_TX);
	
}
