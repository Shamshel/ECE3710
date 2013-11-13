; main.s
; Runs on any Cortex M processor
; A very simple first project implementing a random number generator
; Daniel Valvano
; May 4, 2012

;  This example accompanies the book
;  "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
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
       AREA    DATA, ALIGN=2
       ALIGN          
       AREA    |.text|, CODE, READONLY, ALIGN=2
	   
; offsets

; GPIO
GPIODATA	EQU	0x3FC
GPIODIR		EQU	0x400
GPIOIS		EQU	0x404
GPIOIBE		EQU	0x408
GPIOIEV		EQU	0x40C
GPIOIM		EQU	0x410
GPIORIS		EQU	0x414
GPIOMIS		EQU	0x418
GPIODR2R	EQU	0x500
GPIODR4R	EQU	0x504
GPIODR8R	EQU	0x508
GPIOODR		EQU	0x50C
GPIOPUR		EQU 0x510
GPIOPDR		EQU	0x514
GPIODEN		EQU	0x51C
GPIOLOCK	EQU	0x520
GPIOCR		EQU 0x524

; Timer
GPTMCFG		EQU 0x000	; GPTM config
GPTMTAMR	EQU 0x004	; Timer A mode
GPTMTBMR	EQU 0x008	; Timer B mode
GPTMCTL		EQU	0x00C	; GPTM control
GPTMRIS		EQU 0x01C	; interrupt status
GPTMICR		EQU 0x024	; interrupt clear
GPTMTAILR	EQU	0x028	; Timer A interval
GPTMTBLRR	EQU	0x02C	; Timer b interval

; base addrs

; GPIO
GPIOA	DCD	0x40004000
GPIOB	DCD	0x40005000
GPIOC	DCD	0x40006000
GPIOD	DCD	0x40007000
GPIOE	DCD	0x40024000
GPIOF	DCD	0x40025000

; timers
TMR0	DCD 0x40030000
TMR1	DCD 0x40031000
TMR2	DCD 0x40032000
TMR3	DCD 0x40033000
TMR4	DCD 0x40034000
TMR5	DCD 0x40035000
WTMR0	DCD 0x40036000
WTMR1	DCD 0x40037000
WTMR2	DCD 0x4004C000
WTMR3	DCD	0x4004D000
WTMR4	DCD	0x4004E000
WTMR5	DCD	0x4004F000


RCGCGPIO	DCD	0x400FE108
RCGC1		DCD	0x400FE104

;arbitrary init values
CLKSET		DCD	0x01	; init port A
TMRSET		DCD	0x10000
PORTAOUTSET	DCD	0x0C	; 2 is output
PORTAINSET	DCD 0x80	; 3:6 is input

GPTMCTLMASK DCD 0xFFFF6F7F ; clear GPTMCTL
GPTMCFGMASK	DCD	0xFFFFFFF8	; clear GPTMCFG
GPTMTAMRMASK	DCD 0xFFFFFFFC ; clear GPTMTAMR

; .005 seconds *16000000 cycles = 80000 cycles = 0x13880 cycles
TIMEOUT		DCD	0x13880

       EXPORT  Start
	   
; assume the following:
;	window 1,2, vent, and door sensors are connected
;	to ports PA3:6 respectively
;	Motion detector is connected to PA7.
;	Alarm is attached to PA2
Start  
	; enable clock for GPIO
	LDR r0, RCGCGPIO
	LDR r1, CLKSET
	STR r1, [r0]
	
	; enable clock for TMR0
	LDR r0, RCGC1
	LDR r1, TMRSET
	STR r1, [r0]
	
	; initialize GPIOA
	LDR r0, GPIOA
	LDR r1, PORTAOUTSET
	LDR r2, PORTAINSET
	BL GPIO_init
	
	; initialize TMR0
	LDR r0, TMR0
	LDR r1, TIMEOUT
	BL TMR_init
	
	MOV r2, #0x00
	
	B Main

; TMR setup
; 1. disable TnEN of GPTMCTL
; 2. write GPTMCFG with 0x00000000
; 3. configure TnMR in GPTMTnMR
;	(one-shot 0x01, periodic 0x02)
; 4. configure the remainder of GPTMTnMR
; 5. load interval into GPTMTnILR
; 6. configure interrupts (GPTMIMR)
; 7. set TnEN in GPTMCTL to enable
; 8. poll GPTMRIS

; r0 stores base address, r1 stores counter value.
; configures tmr0 for periodic concatenated (32-bit) operation
TMR_init
	LDR r2, [r0, #GPTMCTL] ; read
	LDR r3, GPTMCTLMASK
	AND r2, r2, r3 ; modify
	STR r2, [r0, #GPTMCTL] ; write
	LDR r2, [r0, #GPTMCFG] ; read
	LDR r3, GPTMCFGMASK
	AND r2, r2, r3 ; modify
	STR r2, [r0, #GPTMCFG] ; write
	LDR r2, [r0, #GPTMTAMR] ; read
	LDR r3, GPTMTAMRMASK
	AND r2, r2, r3 ; modify
	ADD r2, r2, #0x02 ; 0x02=periodic
	STR r2, [r0, #GPTMTAMR] ; write
	STR r1, [r0, #GPTMTAILR]
	MOV r2, #0x01 ; enable timer A
	STR r2, [r0, #GPTMCTL]
	BX lr
	
; r0 stores base address, r1 stores out val, r2 stores in val, r3 will store the or of the two
GPIO_init
	ORR r3, r2, r1			; store active pins
	STR r1, [r0, #GPIODIR] 	; set directions
	STR r3, [r0, #GPIODR2R]	; set power (2mA)
	STR	r1, [r0, #GPIOODR]	; set outputs open drain
	STR r2, [r0, #GPIOPUR]	; set inputs to pull ups
	STR r3, [r0, #GPIODEN]	; enable pins
	BX lr

; main loop
; r0 is addr storage, r1 is ldr storage, r2 stores the count
; r3 stores the previous state of the motion sensor, r4 stores the number of
; transistions in 50 ms.  if greater than 3, trip.
Main
	 ;check timeout
	 LDR r0, TMR0
	 LDR r1, [r0, #GPTMRIS]
	 ANDS r1, #0x01 ; sets z if zero
	 BEQ Main
	 ; clear expiry flag
	 MOV r1, #1
	 STR r1, [r0, #GPTMICR]
	 ADD r2, #1
	 CMP r2, #20
	 
	 LDR r0, GPIOA
	 
	 BEQ hundredMsCheck
	 
	 CMP r2, #15
	 
	 BEQ seventyFiveMsCheck
	 
	 CMP r2, #10
	 
	 BEQ fiftyMsCheck
	 
	 CMP r2, #5
	 
	 BEQ twentyFiveMsCheck
	 
	 B fiveMsCheck
	 
; check window sensors
hundredMsCheck
	LDR r2, [r0, #GPIODATA]
	AND r2, #0x18
	CMP r2, #0x00
	MOV r5, #0x04
	IT NE ; if either A3 or A4 are high, trip
		STRNE r5, [r0, #GPIODATA]

	B fiftyMsCheck
	
; check vent sensor
seventyFiveMsCheck
	LDR r2, [r0, #GPIODATA]
	AND r2, #0x20
	CMP r2, #0x00
	MOV r5, #0x04
	IT NE ; check if A5 went high, if so, trip
		STRNE r5, [r0, #GPIODATA]

	B twentyFiveMsCheck
	
; reset transistion count
fiftyMsCheck
	MOV r4, #0x00
	
	B twentyFiveMsCheck

; check door sensor
twentyFiveMsCheck
	LDR r2, [r0, #GPIODATA]
	AND r2, #0x40
	MOV r5, #0x04
	IT NE ; check if A6 went high, if so, trip
		STRNE r5, [r0, #GPIODATA]
	
	B fiveMsCheck

fiveMsCheck
	; check state transistion on sensor
	LDR r0, GPIOA
	LDR r2, [r0, #GPIODATA]
	AND r2, #0x80
	CMP r2, r3
	IT NE
		ADDNE r4, #1
	
	MOV r3, r2
	MOV r5, #0x04
	CMP r4, #3 ; if more than 3 transisitions, trip
	IT HI
		STRHI r5, [r0, #GPIODATA]
	B Main
       ALIGN      
       END  
           