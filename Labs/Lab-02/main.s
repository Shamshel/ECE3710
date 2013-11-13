; main.s
; Runs on any Cortex M processor
;
; This program drives a ten segment LED bar in
; a binary counter fashion.
;
; Included are three buttons, two onboard and
; one external.  These buttons control the start
; stop, and restart functions.
;
; The bar graph is attached to ports B0:7 and A2:3 and is active low.
;
; The button is connected between ground and A7.  There is no pull resistor.
; The dev board switches are connnected to ports F0 and F4


       THUMB
       AREA    DATA, ALIGN=2
       ALIGN          
       AREA    |.text|, CODE, READONLY, ALIGN=2
       EXPORT  Start
	   
; offsets
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

; base addrs
GPIOA	DCD	0x40004000
GPIOB	DCD	0x40005000
GPIOC	DCD	0x40006000
GPIOD	DCD	0x40007000
GPIOE	DCD	0x40024000
GPIOF	DCD	0x40025000

;arbitrary init values
CLKSET		DCD	0x23	; init ports A, B, and F
PORTAOUTSET	DCD	0x0C	; 2:3 are output
PORTAINSET	DCD 0x80	; 6:7 is input
PORTBOUTSET	DCD	0xFF	; 0:7 are output
PORTBINSET	DCD 0x00	; no input on B
PORTFOUTSET	DCD 0x02	; 1 is output
PORTFINSET	DCD 0x11	;0, 4 are input

RCGCGPIO	DCD	0x400FE108

SETUP	DCD 0x02		; pin 1 init
	   
Start
	; init gpios: see pg 651 of the TI datasheet.
	; set up RCGCGPIO
	LDR r0, RCGCGPIO
	LDR r1, CLKSET
	STR r1, [r0]
	
	; init GPIOA
	LDR r0, GPIOA ; load port A base
	LDR r1, PORTAOUTSET
	LDR r2, PORTAINSET
	BL GPIO_init
	
	; turn off LEDs
	MOV r1, #0x0C
	STR r1, [r0, #GPIODATA]
	
	; init GPIOB
	LDR r0, GPIOB
	LDR r1, PORTBOUTSET
	LDR r2, PORTBINSET
	BL GPIO_init
	
	MOV r1, #0xFF
	STR r1, [r0, #GPIODATA]
	
	; init GPIOF
	; TI felt it wise to lock
	; port F... unlock it
	LDR r0, GPIOF
	LDR r1, =0x4C4F434B
	STR r1, [r0, #GPIOLOCK]
	
	; enable commit to all pins
	MOV r1, #0xFF
	STR r1, [r0, #GPIOCR]
	
	;back to the good stuff:	
	LDR r1, PORTFOUTSET
	LDR r2, PORTFINSET
	BL GPIO_init
	STR r3, [r0, #GPIOPUR] ; enable onboard LED pull-up
	
	MOV32 r1, #0x02 ; turn on onboard LED
	STR r1, [r0, #0x3FC]
	
	; setup complete, continue with application
	MOV r4, #0x00
	MOV r5, #0x7A0000
	ADD r5, #0x001200
	MOV r1, #0x00
	MOV r2, #0x400
	B main
	
GPIO_init ; r0 stores base address, r1 stores out val, r2 stores in val, r3 will store the or of the two
	ORR r3, r2, r1			; store active pins
	STR r1, [r0, #GPIODIR] 	; set directions
	STR r3, [r0, #GPIODR2R]	; set power (8mA)
	STR	r1, [r0, #GPIOODR]	; set outputs open drain
	STR r2, [r0, #GPIOPUR]	; set inputs to pull ups
	STR r3, [r0, #GPIODEN]	; enable pins
	BX lr
	
; let SW1=Start, SW2=Stop, EXT_SWITCH=RST
; r0=addresses, r1=count, r2=RST_VAL, r3=tmp, r4=delayCount, r5=DELAY_MAX, r6=another tmp
main
	; check Stop (port F0)
	LDR r0, GPIOF
	LDR r3, [r0, #GPIODATA]
	AND r3, r3, #0x01 ; and off the LED and SW1 data
	CMP r3, #0x01
	BNE stopLoop
	
	; check reset (port A7)
	LDR r0, GPIOA
	LDR r3, [r0, #GPIODATA]
	AND r3, r3, #0x80 ; and off MSb of LEDs
	CMP r3, #0x80
	BNE resetCount
	
	; check delay done
	CMP r4, r5
	BHS incrementCount
	
	; if this has been reached, increment delay
	ADD r4, #15
	
	B main
	
stopLoop
	; wait for button to be released
	LDR r3, [r0, #GPIODATA]
	AND r3, r3, #0x01
	CMP r3, #0x01
	BNE stopLoop
	
	; button released, check for start (F4=0)
	LDR r3, [r0, #GPIODATA]
	AND r3, r3, #0x10
	CMP r3, #0x10
	BEQ stopLoop
	
	B main
	
	
resetCount
	MOV r1, #0x00
	B main
	
incrementCount
	ADD r1, #0x01 ; increment count
	MOV r4, #0x00 ; reset delay
	
	; update LEDs
	; Bit 1 will correspond to LED 1 on the bar.
	LDR r0, GPIOB
	MVN r3, r1 ; not-move count (LEDs are active low)
	AND r6, r3, #0xFF ; only writing botom 8 bits here
	STR r6, [r0, #GPIODATA]
	
	LDR r0, GPIOA
	AND r6, r3, #0x300 ; only writing top 2 bits
	LSR r6, r6, #0x06 ; shift MSbs into position
	STR r6, [r0, #GPIODATA]
	
	; toggle LED
	LDR r0, GPIOF ; read
	LDR r3, [r0, #0x3FC]
	AND r3, r3, #0x02 ; modify
	MVN r6, r3
	STR r6, [r0, #0x3FC]
	
	; check count done
	CMP r1, #0x400 ; 2^11
	BHS resetCount
	
	B main
	
       ALIGN      
       END  
           