; main.s
; Runs on LM3S1968
; Project files adapted from:
; Daniel Valvano
; May 4, 2012

;  This example accompanies the book
;  "Embedded Systems: Introduction to the Arm Cortex M3",
;  ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2012
;  Section 3.3.10, Program 3.12
;
;Copyright 2012 by Jonathan W. Valvano, valvano@mail.utexas.edu
;   You may use, edit, run or distribute this file
;   as long as the above copyright notice remains
;THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
;OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
;MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
;VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
;OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
;For more information about my classes, my research, and my books, see
;http://users.ece.utexas.edu/~valvano/


       THUMB
       AREA    |.text|, CODE, READONLY, ALIGN=2
       EXPORT  Start
Start
   MOV r0, #0
   
   B Delay

Delay
   ADD r0, #1
   CMP r0, #0x186000
   BLT Delay
   B Fin

Fin
   B Fin 
	   

       ALIGN      
       END  
           