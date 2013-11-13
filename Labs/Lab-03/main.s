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
       EXPORT  Start
	   
; base addrs
; gpio
GPIOA		DCD	0x40004000
GPIOB		DCD	0x40005000
GPIOC		DCD	0x40006000
GPIOD		DCD	0x40007000
GPIOE		DCD	0x40024000
GPIOF		DCD	0x40025000
	   
; uart
UART0		DCD	0x4000C000
UART1		DCD	0x4000D000
UART3		DCD	0x4000E000
UART4		DCD	0x4000F000
UART5		DCD	0x40010000
UART6		DCD 0x40012000
UART7		DCD	0x40013000

SYSCTL		DCD	0x400FE000
;RCGCUART 	DCD	0x400FE618	
;RCGCGPIO	DCD 0x400FE608	
	 
; offsets

RCGCGPIO	EQU	0x618 ; I/O run mode clock gating control
RCGCUART	EQU	0x608 ; UART clock enable reg

; gpio
GPIODATA	EQU	0x3FC
GPIODIR		EQU	0x400
GPIOIS		EQU	0x404
GPIOIBE		EQU	0x408
GPIOIEV		EQU	0x40C
GPIOIM		EQU	0x410
GPIORIS		EQU	0x414
GPIOMIS		EQU	0x418
GPIOAFSEL	EQU	0x420
GPIODR2R	EQU	0x500
GPIODR4R	EQU	0x504
GPIODR8R	EQU	0x508
GPIOODR		EQU	0x50C
GPIOPUR		EQU 	0x510
GPIOPDR		EQU	0x514
GPIODEN		EQU	0x51C
GPIOLOCK	EQU	0x520
GPIOCR		EQU 	0x524
GPIOPCTL	EQU	0x52C

; uart
UARTDR		EQU	0x000	; uart data
UARTRSR		EQU 0x004	; recieve status
UARTFR		EQU	0x018	; flag
UARTILPR	EQU	0x020	; IrDA low-power register
UARTIBRD	EQU	0x024	; integer baud rate divisor
UARTFBRD	EQU	0x028	; fractional baud divisor
UARTLCRH	EQU	0x02C	; line control
UARTCTL		EQU	0x030	; control
UARTIFLS	EQU	0x034	; interrupt fifo level select
UARTIM		EQU	0x038	; interrupt mask
UARTRIS		EQU	0x03C	; raw interrupt status
UARTMIS		EQU	0x040	; masked interrupt status
UARTICR		EQU	0x044	; interrupt clear
UARTCC		EQU	0xFC8	; clock configuration

	   
Start



	; 1, 2
	LDR r0, SYSCTL
	MOV r1, #0x02
	STR r1, [r0, #RCGCUART] ; enable UART1
	STR r1, [r0, #RCGCGPIO] ; enable GPIOB
	
	LDR r0, GPIOB
	MOV r1, #0x03
	STR r1, [r0, #GPIOAFSEL] ; enable alternative functionality
	STR r1, [r0, #GPIODEN] ; digital enable
	STR r1, [r0, #GPIODR2R] ; set current level
	MOV r1, #0x011
	STR r1, [r0, #GPIOPCTL] ; configure alternative signal control

	LDR r0, UART1

	BL initUart
	MOV r2, #0x1
	
Tx
	STRB r2, [r0, #UARTDR]
	ADD r2, #1

	;B wait
	
wait
	LDR r1, [r0, #UARTFR]
	ANDS r1, #0x8
	BEQ wait
	B Tx
	

; uart setup r0 holds base address, assume setup for 9600 baud, no parity, 8 transmit bits.
initUart
	   LDR r1, [r0, #UARTCTL]
	   LSR r1, #1
	   LSL r1, #1
	   STR r1, [r0, #UARTCTL] ; disable
	   ; set baud rate
	   ; brd = 16e6/(16*9600) = 104.167
	   ; integer portion = 104=0x68
	   MOV r1, #0x068
	   STR r1, [r0, #UARTIBRD]
	   ; int(0.167*2^6+0.5)=11=0x00B
	   ; fractional portion = 0x0B
	   MOV r1, #0x0B
	   STR r1, [r0, #UARTFBRD]
	   ; write line control
	   ; 8 data bits, no parity, 1 stop bit, no fifo
	   MOV r1, #0x60
	   STR r1, [r0, #UARTLCRH]
	   ; write to clock generation control
	   MOV r1, #0x00 ; system clock
	   STR r1, [r0, #UARTCC]
	   ; enable UART, TX, RX
	   MOV r1, #0x301
	   STR r1, [r0, #0x30]
	   nop
	   nop
	   nop
	   BX lr

       ALIGN      
       END  
           