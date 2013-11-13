

       THUMB
       AREA    DATA, ALIGN=2
       ALIGN          
       AREA    |.text|, CODE, READONLY, ALIGN=2
       EXPORT  Start
	   
GPIOB	DCD	0x40005000

GPIODATA	EQU	0x3FC
GPIODIR		EQU	0x400
GPIODR2R	EQU	0x500
GPIOODR		EQU	0x50C
GPIOPUR		EQU 0x510
GPIODEN		EQU	0x51C

Start  
	; init GPIOB
	LDR r0, GPIOB
	MOV r1, #0x0000
	MOV r2, #0xFFFF
	BL GPIO_init
	
	B LoadTable
	
	B Main
	   
GPIO_init ; r0 stores base address, r1 stores out val, r2 stores in val, r3 will store the or of the two
	ORR r3, r2, r1			; store active pins
	STR r1, [r0, #GPIODIR] 	; set directions
	STR r3, [r0, #GPIODR2R]	; set power (8mA)
	STR	r1, [r0, #GPIOODR]	; set outputs open drain
	STR r2, [r0, #GPIOPUR]	; set inputs to pull ups
	STR r3, [r0, #GPIODEN]	; enable pins
	BX lr
	
Main
	MOV r2, r0
	AND r2, r2, #0x0F ; M1
	
	MOV r3, r0
	LSR r3, r3, #4 ; M2
	
	LDR r1, GPIOB ; tmp
	
	LDR r4, [r1, #GPIODATA]
	MOV r5, r4
	
	AND r4, r4, #0x0F ; K1
	
	LSR r5, r5, #4 ; K2
	
	; (EOR)XOR M1 and K1
	EOR r6, r2, r4
	
	; find substitue from table
	MOV r7, #4
	MUL r7, r6, r7 ; calculate table offset
	LDR r6, [SP, r7]
	
	; (EOR)XOR result with K2
	EOR r6, r6, r5
	
	; store lower 4 bits
	MOV r0, r6
	
	; repeat for upper bits
	; (EOR)XOR M2 and K1
	EOR r6, r3, r4
	
	; find substitue from table
	MOV r7, #4
	MUL r7, r6, r7 ; calculate table offset
	LDR r6, [SP, r7]
	
	; (EOR)XOR result with K2
	EOR r6, r6, r5
	
	; shift bits into position
	LSL r6, r6, #4
	
	; add upper bits
	ADD r0, r0, r6
	
	B Fin
	
LoadTable
	MOV r0, #10
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #2
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #11
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #13
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #6
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #1
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #12
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #8
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #5
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #14
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #9
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #0
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #15
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #3
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #4
	STR r0, [SP]
	
	SUB SP, SP, #4 ; stack grows down
	
	MOV r0, #7
	STR r0, [SP]
	
	MOV r0, #0xA1 ; load dummy data

	B Main
	
Fin
	B Fin

       ALIGN      
       END  
           