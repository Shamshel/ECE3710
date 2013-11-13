

       THUMB
       AREA    DATA, ALIGN=2
       ALIGN          
       AREA    |.text|, CODE, READONLY, ALIGN=2
       EXPORT  Start
	   
GPIOD	DCD	0x40007000

GPIODATA	EQU	0x3FC
GPIODIR		EQU	0x400
GPIODR2R	EQU	0x500
GPIOODR		EQU	0x50C
GPIOPUR		EQU 0x510
GPIODEN		EQU	0x51C

Start  

	; init GPIOD
	LDR r0, GPIOD
	MOV r1, #0x8F
	MOV r2, #0x70
	BL GPIO_init

	; load port d
	LDR r1, [r0, #GPIODATA]
	
	; mov mask
	MOV r2, #0x81
	
	; mask off bits 6:2
	AND r4, r2, r1
	
	; mov mask
	MOV r2, #0x70
	
	AND r3, r2, r1
	
	; add bits 7 and 0 back on
	ADD r3, r4, r3, LSR #3
	
	; write to port d
	STR r3, [r0, #GPIODATA]
	
Fin
	B Fin
	   
GPIO_init ; r0 stores base address, r1 stores out val, r2 stores in val, r3 will store the or of the two
	ORR r3, r2, r1			; store active pins
	STR r1, [r0, #GPIODIR] 	; set directions
	STR r3, [r0, #GPIODR2R]	; set power (8mA)
	STR	r1, [r0, #GPIOODR]	; set outputs open drain
	STR r2, [r0, #GPIOPUR]	; set inputs to pull ups
	STR r3, [r0, #GPIODEN]	; enable pins
	BX lr

       ALIGN      
       END  
           