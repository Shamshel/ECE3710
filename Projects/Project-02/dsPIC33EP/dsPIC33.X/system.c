/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#if defined(__XC16__)
    #include <xc.h>
#elif defined(__C30__)
    #if defined(__dsPIC33E__)
    	#include <p33Exxxx.h>
    #elif defined(__dsPIC33F__)
    	#include <p33Fxxxx.h>
    #endif
#endif


#include <stdint.h>          /* For uint16_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */

#include "system.h"          /* variables/params used by system.c             */

/******************************************************************************/
/* System Level Functions                                                     */
/*                                                                            */
/* Custom oscillator configuration funtions, reset source evaluation          */
/* functions, and other non-peripheral microcontroller initialization         */
/* functions get placed in system.c.                                          */
/*                                                                            */
/******************************************************************************/

/* Refer to the device Family Reference Manual Oscillator section for
information about available oscillator configurations.  Typically
this would involve configuring the oscillator tuning register or clock
switching useing the compiler's __builtin_write_OSCCON functions.
Refer to the C Compiler for PIC24 MCUs and dsPIC DSCs User Guide in the
compiler installation directory /doc folder for documentation on the
__builtin functions.*/

/* TODO Add clock switching code if appropriate.  An example stub is below.   */
void ConfigureOscillator(void)
{
        /* Disable Watch Dog Timer */
        RCONbits.SWDTEN = 0;

        //set up pll multipliers
        //HS primary osc = 16MHz
        //max frequency = 140MHz (70MIPS)
        //boost frequency to 20MHz (needed for SPI module clock divider)
        //Fpllo = Fin*((PLLDIV+2)/((PLLPRE+2)*2(PLLPOST+1)))
        //PLLDIV = 50-2 = 48
        PLLFBD = 0x0030;
        //PLLPRE = 2 (default)
        //PLLPOST = 4 (default)

        /* When clock switch occurs switch to Primary Osc (HS, XT, EC) */
        __builtin_write_OSCCONH(0x03);  /* Set OSCCONH for clock switch */
        __builtin_write_OSCCONL(OSCCON | 0x01);  /* Start clock switching */

        /* Wait for Clock switch to occur */
        while(OSCCONbits.COSC != 0b011);

        /* Wait for PLL to lock */
        while(OSCCONbits.LOCK != 1);

}

