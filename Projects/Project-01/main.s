; main.s
; Runs on any Cortex M processor
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

; Timer
GPTMCFG		EQU 0x000	; GPTM config
GPTMTAMR	EQU 0x004	; Timer A mode
GPTMTBMR	EQU 0x008	; Timer B mode
GPTMCTL		EQU	0x00C	; GPTM control
GPTMRIS		EQU 0x01C	; interrupt status
GPTMICR		EQU 0x024	; interrupt clear
GPTMTAILR	EQU	0x028	; Timer A interval
GPTMTBLRR	EQU	0x02C	; Timer b interval
GPTMTAR		EQU 0x048
GPTMOS		DCD	0x01	; one shot mode
GPTMPM		DCD	0x02	; periodic mode

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

;arbitrary init values
CLKSET		DCD	0x23	; init ports A, B, and F
PORTAOUTSET	DCD	0x0C	; 2:3 are output
PORTAINSET	DCD 0xC0	; 6:7 is input
PORTBOUTSET	DCD	0xFF	; 0:7 are output
PORTBINSET	DCD 0x00	; no input on B
PORTFOUTSET	DCD 0x00	; no output on F
PORTFINSET	DCD 0x0F	; 0:3 are input

GPTMCTLMASK 	DCD 0xFFFF6F7F ; clear GPTMCTL
GPTMCFGMASK		DCD	0xFFFFFFF8 ; clear GPTMCFG
GPTMTAMRMASK	DCD 0xFFFFFFFC ; clear GPTMTAMR

; .25 seconds *16000000 cycles = 600000 cycles = 0x3D0900 cycles
TIMEOUT		DCD	0x3D0900
TIMEOUTSEC	DCD	0xF42400

; game timeout constants
MS320		DCD	0x4E2000
MS80		DCD 0x138800

; RCGC1 register setup
TMRSET		DCD	0x30000

RCGCGPIO	DCD	0x400FE108
RCGC1		DCD	0x400FE104

SETUP	DCD 0x02		; pin 1 init
	   
Start
	; init gpios: see pg 651 of the TI datasheet.
	; set up RCGCGPIO
	LDR r0, RCGCGPIO
	LDR r1, CLKSET
	STR r1, [r0]
	
	; enable clock for TMR0
	LDR r0, RCGC1
	LDR r1, TMRSET
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
	
	; init timer 0
	; configure for one-shot
	LDR r0, TMR0
	LDR r1, TIMEOUT
	LDR r2, GPTMPM
	BL TMR_init
	
	
	; init timer 1
	; configure for periodic
	LDR r0, TMR1
	LDR r1, TIMEOUTSEC
	LDR r2, GPTMPM
	BL TMR_init
	
	; setup complete, continue with application
	
	B main
	
GPIO_init ; r0 stores base address, r1 stores out val, r2 stores in val, r3 will store the or of the two
	ORR r3, r2, r1			; store active pins
	STR r1, [r0, #GPIODIR] 	; set directions
	STR r3, [r0, #GPIODR2R]	; set power (2mA)
	STR	r1, [r0, #GPIOODR]	; set outputs open drain
	STR r2, [r0, #GPIOPDR]	; set inputs to pull downs
	STR r3, [r0, #GPIODEN]	; enable pins
	BX lr
	
	; enable clock for TMR1
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

; r0 stores base address, r1 stores counter value, r2 stores mode.
; configures timer for periodic concatenated (32-bit) operation
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
	; clear expiry flag
	MOV r2, #1
	STR r2, [r0, #GPTMICR]
	NOP
	NOP
	
	BX lr
	
; 
; 

updateLEDs
	; push positions to LEDs
	ORR r4, r2, r3
	CMP r4, #0x100
	MOV r5, #0x00
	IT HI
		ANDHI r5, r4, #0x300
	
	LSR r5, #6
	MVN r5, r5
	
	AND r4, #0xFF
	MVN r4, r4
	
	LDR r0, GPIOB
	STR r4, [r0, #GPIODATA]
	
	LDR r0, GPIOA
	STR r5, [r0, #GPIODATA]

	BX lr

main
	; program begin, start at step 3 (as in a reset)
	; flash LED's 4:5 at 2 Hz until button press detected
	MOV r2, #0x00
	BL playerReadyLoop
	
	; set user LEDs
	LDR r0, GPIOB
	MOV r1, #0xCF
	STR r1, [r0, #GPIODATA]
	
	; begin main game sequence
	; r2=p1 pos, r3=p2 pos, begin in center
	; r7 is draw counter
	MOV r2, #0x10
	MOV r3, #0x20
	MOV r7, #0
	B gameLoop
	
FinPrep
	MOV r6, r2
	MOV r7, r3
	
	; load timer with new value, wait for
	; timeout or response
	MOV r1, #0
	LDR r0, TMR0
	; disable timer 0
	STR r1, [r0, #GPTMCTL]
	; change timeout
	LDR r1, TIMEOUT
	STR r1, [r0, #GPTMTAILR]
	;enable timer 0
	MOV r1, #0x01
	STR r1, [r0, #GPTMCTL]
	; clear interrupt
	STR r1, [r0, #GPTMICR]
	
	B Fin
	
Fin
	LDR r0, TMR0
	LDR r1, [r0, #GPTMRIS]
	AND r1, #0x01
	CMP r1, #0x01
	BNE Fin
	
	; clear interrupt
	STR r1, [r0, #GPTMICR]
	
	AND r2, r6
	AND r3, r7
	
	CMP r2, r6
	ITTEE EQ
		MVNEQ r2, r2
		MVNEQ r3, r3
		ORRNE r2, r6
		ORRNE r3, r7
		
	AND r2, r6
	AND r3, r7
	
	BL updateLEDs

	B Fin
	
gameLoop
	; check for end of game condition
	
	; p1 victory?
	CMP r2, #0x100
	BEQ FinPrep
	
	; p2 victory?
	CMP r3, #0x2
	BEQ FinPrep
	
	; grab random number, add one second
	LDR r0, TMR1
	LDR r4, [r0, #GPTMTAR]
	LDR r5, TIMEOUTSEC
	ADD r4, r4, r5
	MOV r1, #0
	LDR r0, TMR0
	; disable timer 0
	STR r1, [r0, #GPTMCTL]
	; change timeout
	STR r4, [r0, #GPTMTAILR]
	;enable timer 0
	MOV r1, #0x01
	STR r1, [r0, #GPTMCTL]
	; clear interrupt
	STR r1, [r0, #GPTMICR]
	MOV r1, #0
	; wait for timeout
	BL waitForTimeout
	; clear timeout
	MOV r1, #1
	STR r1, [r0, #GPTMICR]
	; move users back
	LSR r2, #1
	LSL r3, #1
	
	BL updateLEDs
	
	B waitForPlayerMove
	
	
waitForPlayerMove	
	; read input buttons
	LDR r0, GPIOA
	LDR r1, [r0, #GPIODATA]
	AND r1, r1, #0xC0
	; check if player 1 moved first
	CMP r1, #0x40
	BEQ P2Counter
		
	
	; check if player 2 moved first
	CMP r1, #0x80
	BEQ P1Counter
	
	; neither player moved, wait
	B waitForPlayerMove

P1Counter
	; p2 has responded, wait for player 1 to respond within timeout
	
	; move p2 right as per move
	LSR r3, #1
	BL updateLEDs
	
	; get player response multipliers
	; multipliers are 2-bit numbers, per
	; each player
	; pins are on PF.0:3
	
	LDR r0, GPIOF
	LDR r4, [r0, #GPIODATA]
	; let bits PF.0:1 be player 1's mult bits
	AND r4, r4, #0x03
	
	; player delay is 2^(-min(d, 4))*(320-80*Sn) ms
	CMP r7, #4
	
	ITE HI
		MOVHI r8, r7
		MOVLS r8, #4
	
	LDR r9, MS320
	LDR r10, MS80
	MUL r10, r4, r10 ; 80*Sn
	SUB r9, r9, r10 ; 320-80*Sn
	
	LSR r9, r8 ; 2^(-r8)*(320-80*Sn)
	
	; load timer with new value, wait for
	; timeout or response
	MOV r1, #0
	LDR r0, TMR0
	; disable timer 0
	STR r1, [r0, #GPTMCTL]
	; change timeout
	STR r9, [r0, #GPTMTAILR]
	;enable timer 0
	MOV r1, #0x01
	STR r1, [r0, #GPTMCTL]
	; clear interrupt
	STR r1, [r0, #GPTMICR]
	
	; let r4 be the response mask
	MOV r4, #0x40
	
	BL waitForAction
	
	; player 1 failed to respond, advance player 2
	LSR r3, #1
	
	; round ended with victory, reset drawcount
	MOV r7, #0
	
	BL updateLEDs
	
	B gameLoop

P2Counter

	; move player 1 as per move
	LSL r2, #1
	
	BL updateLEDs

	; get player response multipliers
	; multipliers are 2-bit numbers, per
	; each player
	; pins are on PF.0:3
	
	LDR r0, GPIOF
	LDR r4, [r0, #GPIODATA]
	AND r4, r4, #0x0C
	LSR r4, #2

	; let bits PF.2:3 be player 2's mult bits
	; let r4 store player 2's mult bits
	; let r7 store number of drawn moves
	LDR r0, GPIOF
	LDR r4, [r0, #GPIODATA]
	AND r4, r4, #0x0C
	LSR r4, #2
	
	; player delay is 2^(-min(d, 4))*(320-80*Sn) ms
	CMP r7, #4
	
	ITE HI
		MOVHI r8, r7
		MOVLS r8, #4
	
	LDR r9, MS320
	LDR r10, MS80
	MUL r10, r4, r10 ; 80*Sn
	SUB r9, r9, r10 ; 320-80*Sn
	
	LSR r9, r8 ; 2^(-r8)*(320-80*Sn)
	
	; load timer with new value, wait for
	; timeout or response
	MOV r1, #0
	LDR r0, TMR0
	; disable timer 0
	STR r1, [r0, #GPTMCTL]
	; change timeout
	STR r9, [r0, #GPTMTAILR]
	;enable timer 0
	MOV r1, #0x01
	STR r1, [r0, #GPTMCTL]
	; clear interrupt
	STR r1, [r0, #GPTMICR]
	
	; let r4 be the response mask
	MOV r4, #0x80
	
	BL waitForAction
	
	; player 2 failed to respond, advance player 1
	LSL r2, #1
	
	; round ended with victory, reset drawcount
	MOV r7, #0
	
	BL updateLEDs
	
	B gameLoop
	
waitForAction
	; check timeout
	LDR r0, TMR0
	LDR r1, [r0, #GPTMRIS]
	AND r1, #1
	CMP r1, #0x01
	
	; if timeout return to counter function
	BEQ returnPlayerCounter
	
	; check player response
	LDR r0, GPIOA
	LDR r1, [r0, #GPIODATA]
	
	; and off other data
	AND r1, r1, r4
	
	CMP r1, r4
	BNE waitForAction ; no response
	
	; player responded
	; based on mask, move respective player back into position
	CMP r1, #0x40
	ITE EQ
		LSLEQ r2, #1
		LSRNE r3, #1
		
	; no need to return to previous function, BL is fine
	BL updateLEDs
	
	; since player responded, increment draw reg
	ADD r7, #1
	
	B gameLoop
	
returnPlayerCounter
	BX lr
	
	
waitForTimeout
	; else, check timeout
	LDR r0, TMR0
	LDR r1, [r0, #GPTMRIS]
	MOV r4, #0x01
	AND r1, r1, r4
	CMP r1, #0x01
	BEQ returnGameLoop
	B waitForTimeout
	
returnGameLoop
	BX lr

playerReadyLoop
	; check inputs, LED4=p1, LED5=p2
	; A6=p1 button, A7=p2 button
	LDR r0, GPIOA
	LDR r1, [r0, #GPIODATA]
	AND r1, r1, #0xC0
	LSR r1, r1, #2 ; shift bits into position
	ORR r2, r1 ; or into response flag
	LDR r0, GPIOB
	MOV r1, #0xFF
	CMP r2, #0x30 ; check if both players have responded
	ITT EQ
		LDREQ r1, [r0, #GPIODATA]
		BEQ returnMain
		
	; else, check timeout
	LDR r0, TMR0
	LDR r1, [r0, #GPTMRIS]
	MOV r3, #0x01
	AND r1, r1, r3
	CMP r1, #0x01 ; sets z if zero
	BEQ toggleWaitLeds
	
	; else, return to ready loop
	B playerReadyLoop
	
returnMain
	BX lr
	
toggleWaitLeds
	; timer tripped, reset expiry flag
	MOV r1, #1
	STR r1, [r0, #GPTMICR]
	
	; toggle LEDs 4:5 (based on response flags)
	LDR r0, GPIOB
	LDR r1, [r0, #GPIODATA]
	MVN r1, r1
	AND r1, #0xFF
	ORR r1, #0xCF ; active low
	MVN r3, r2
	AND r3, #0xFF
	AND r1, r1, r3 ; keep led on if player responded
	
	
	LDR r0, GPIOB
	STR r1, [r0, #GPIODATA]

	B playerReadyLoop
	
       ALIGN      
       END  
           