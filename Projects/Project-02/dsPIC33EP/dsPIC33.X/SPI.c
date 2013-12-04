//SPI.c

#include "SPI.h"

void spi_init()
{
    //configure SPI for master mode

    //see page 52 of the NRF24L01+ datasheet for timing
    //let
    //  CKP = 0
    //  CKE = 1
    //  SMP = 0

    //pinout:
    //_SS1=4
    //SCK1=16
    //SDO1=17
    //SDI1=18

    //IFS0bits.SPI1IF = 0; // Clear the Interrupt flag
    //IEC0bits.SPI1IE = 0; // Disable the interrupt

    // SPI1CON1 Register Settings
    SPI1CON1bits.DISSCK = 0; // Internal serial clock is enabled
    SPI1CON1bits.DISSDO = 0; // SDOx pin is controlled by the module
    SPI1CON1bits.MODE16 = 0; // Communication is byte-wide (8 bits)
    SPI1CON1bits.SMP = 0; // Input data is sampled at the middle of data output time
    SPI1CON1bits.CKE = 1; // Serial output data changes on transition from
    
    // active clock state to idle clock state
    SPI1CON1bits.CKP = 0; // Idle state for clock is a low-level;
    // active state is a high-level
    SPI1CON1bits.MSTEN = 1; // Master mode enabled

    //Clock Configuration
    //20MHz input, to 2MHz baud
    //Fsck = Fp/(PPRE*SPRE)
    SPI1CON1bits.SPRE = 5;
    SPI1CON1bits.PPRE = 2;

    //Activate SPI
    SPI1STATbits.SPIEN = 1;

    // Interrupt Controller Settings
    //IFS0bits.SPI1IF = 0; // Clear the Interrupt flag
    //IEC0bits.SPI1IE = 1; // Enable the interrupt

}
