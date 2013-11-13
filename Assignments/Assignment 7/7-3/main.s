; main.s
; Runs on LM3S1968
; Project files adapted from:
; Daniel Valvano
; May 4, 2012

       THUMB
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

; ADC
ADCACTSS	EQU 0x000
ADCRIS		EQU	0x004
ADCIM		EQU	0x008
ADCISC		EQU 0x00C
ADCOSTAT	EQU	0x010
ADCEMUX		EQU	0x014
ADCUSTAT	EQU	0x018
ADCSSPRI	EQU	0x020
ADCPSSI		EQU	0x028
ADCSAC		EQU	0x030
ADCSSMUX0	EQU	0x040
ADCSSCTL0	EQU	0x044
ADCSSFIFO0	EQU	0x048
ADCSSFSTAT0	EQU	0x04C
ADCSSMUX1	EQU	0x060
ADCSSCTL1	EQU	0x064
ADCSSFIFO1	EQU	0x068
ADCSSFSTAT1	EQU	0x06C
ADCSSMUX2	EQU	0x080
ADCSSCTL2	EQU	0x084
ADCSSFIFO2	EQU	0x088
ADCSSFSTAT2	EQU	0x08C
ADCSSMUX3	EQU	0x0A0
ADCSSCTL3	EQU 0x0A4
ADCSSFIFO3	EQU 0x0A8
ADCSSFSTAT3	EQU	0x0AC
ADCTMLB		EQU	0x100

; SYSCTL
RCGCGPIO	DCD	0x400FE108
RCGC0		DCD 0x400FE100

; base addrs

; GPIO
GPIOA	DCD	0x40004000
GPIOB	DCD	0x40005000
GPIOC	DCD	0x40006000
GPIOD	DCD	0x40007000
GPIOE	DCD	0x40024000
GPIOF	DCD	0x40025000

; ADC
ADCBASE DCD 0x40038000

;project specific defines
MEMLOC		DCD	0x2000C000
ADCCLKSET	DCD 0x10000
GPIOCLKSET	DCD	0x01
PORTAOUTSET DCD 0x00
PORTAINSET 	DCD 0x01
	   
       EXPORT  Start
	   
; Because the input from the comparator is not given a specific port,
; assume it is attached to PA0

; Let Vi be attached to the adc0 input pin
; (pin 1 on the 100LQFP footprint; see page 394-395 of the datasheet).
Start
	; enable clock for GPIO
	LDR r0, RCGCGPIO
	LDR r1, GPIOCLKSET
	STR r1, [r0]
	
	; enable clock for ADC0
	LDR r0, RCGC0
	LDR r1, ADCCLKSET
	STR r1, [r0]
	
	; initialize GPIOA
	LDR r0, GPIOA
	LDR r1, PORTAOUTSET
	LDR r2, PORTAINSET
	BL GPIO_init
	
	; initialize ADC0
	BL ADC_init
	
	MOV r2, #0x1FF
	MOV r3, #0x00
	MOV r4, #0x3FF
	
; continuously determine analog value through Successive Approximation Conversion
; r2 stores current code, r3 stores lower code, r4 stores upper code
SAC
	; if r2 == r4 | r2 == r3, assume the code is found
	; (rounding error could cause strange results)
	CMP r2, r4
	BEQ checkADC
	
	CMP r2, r3
	BEQ checkADC

	; check if PA0 is 0 (DAC greater than)
	LDR r0, GPIOA
	LDR r1, [r0, #GPIODATA]
	CMP r1, #0x00
		
	; dac is greater than analog voltage
	; select lower half
	IT EQ
		MOVEQ r4, r2
		
	; dac is less than analog voltage
	; select upper half	
	IT NE
		MOVNE r3, r2
		
	; calculate median of upper and lower
	SUB r2, r4, r3 ; r2 = upper - lower
	MOV r5, #2
	UDIV r2, r5 ; r2 = (upper-lower)/2
	SUB r2, r4, r2 ; r2 = r4 - (upper-lower)/2
	
	; store value to DAC
	LDR r0, MEMLOC
	STR r2, [r0]
	
	B SAC ; wash, rinse, recurse

; check to see if current code is equal to ADC
checkADC
	; begin ADC sequence 0
	LDR r0, ADCBASE
	MOV r1, #0x01
	STR r1, [r0, #ADCPSSI]
	
; wait for conversion to finish
conversionComplete
	LDR r1, [r0, #ADCRIS]
	CMP r1, #0x01
	BNE conversionComplete
	
compareSAC
	; clear interrupt
	MOV r1, #0x01
	STR r1, [r0, #ADCISC]
	
	; compare ADC to SAC
	LDR r1, [r0, #ADCSSFIFO0]
	CMP r1, r2
	; compare is equivalent, routine is finished
	BEQ pushCode
	
	; compare failed, reset and start again
	MOV r2, #0x00
	B SAC
	
pushCode
	PUSH{r2}
	B Fin
	
; r0 stores base address, r1 stores out val, r2 stores in val, r3 will store the or of the two
GPIO_init
	ORR r3, r2, r1			; store active pins
	STR r1, [r0, #GPIODIR] 	; set directions
	STR r3, [r0, #GPIODR2R]	; set power (2mA)
	STR	r1, [r0, #GPIOODR]	; set outputs open drain
	STR r2, [r0, #GPIOPUR]	; set inputs to pull ups
	STR r3, [r0, #GPIODEN]	; enable pins
	BX lr
	

ADC_init
	LDR r0, ADCBASE
	MOV r1, #0x00 ; disable ADC
	STR r1, [r0, #ADCACTSS]
	MOV r1, #0x0 
	STR r1, [r0, #ADCEMUX] ; software trigger
	STR r1, [r0, #ADCSSMUX0] ; input source
	STR r1, [r0, #ADCSSCTL0] ; sample control bits
	MOV r1, #0x01 ; enable ADC0
	STR r1, [r0, #ADCACTSS]
	BX lr

Fin
	B Fin

       ALIGN      
       END  
           