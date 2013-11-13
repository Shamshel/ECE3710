; main.s
; Runs on any Cortex M processor

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
GPIOAFSEL	EQU 0x420
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
GPTMIMR		EQU	0x018	; interrupt mask
GPTMRIS		EQU 0x01C	; interrupt status
GPTMICR		EQU 0x024	; interrupt clear
GPTMTAILR	EQU	0x028	; Timer A interval
GPTMTBLRR	EQU	0x02C	; Timer b interval
GPTMTAR		EQU 0x048

; sysctl
RIS 		EQU 0x050
IMC 		EQU 0x054
MISC 		EQU 0x058
RCC 		EQU 0x060
PLLCFG 		EQU 0x064
RCC2 		EQU 0x070
RCGC0 		EQU 0x100
RCGC1 		EQU 0x104
RCGC2 		EQU 0x108

; base addrs

; GPIO
GPIOA	DCD	0x40004000
GPIOB	DCD	0x40005000
GPIOC	DCD	0x40006000
GPIOD	DCD	0x40007000
GPIOE	DCD	0x40024000
GPIOF	DCD	0x40025000

; timers
GPTM0	DCD 0x40030000
GPTM1	DCD 0x40031000
GPTM2	DCD 0x40032000
GPTM3	DCD 0x40033000

; sysctl
SYSCTL 	DCD	0x400FE000

; application specific defines
TIMEOUTCOUNT	DCD 0x20000404
LIGHT_STATE		DCD 0x20000408
RED_STATE		EQU	0x00 ; red->green
GREEN_STATE		EQU	0x01 ; green->yellow
YELLOW_STATE	EQU	0x02 ; yellow->red
RED_LIGHT		EQU	0x01
YELLOW_LIGHT	EQU	0x04
GREEN_LIGHT		EQU	0x02

	   
; red->PC.0, green->PC.1, yellow->PC.2
; 5KHz square->PB.0
	   
Start  
	; enable clocking to GPIOB:C
    LDR r0, SYSCTL
	MOV r1, #0x06
	STR r1, [r0, #RCGC2]
	
	; enable clocking to TMR0
	MOV r1, #0x10000
	STR r1, [r0, #RCGC1]
	
	; enable digital output on PC.0:2
	LDR r0, GPIOC
	MOV r1, #0x07
	MOV r2, #0x00
	BL GPIO_init
	
	; enable AF on PB.0
	LDR r0, GPIOB
	MOV r1, #0x01
	STR r1, [r0, #GPIOAFSEL]
	
	; enable GPTM0/A edge count mode
	LDR r0, GPTM0
	MOV r1, #25000
	BL GPTM_Edge_Count_init
	
	; initialize vars
	MOV r1, #0x00
	LDR r0, TIMEOUTCOUNT
	STR r1, [r0]
	
	LDR r0, LIGHT_STATE
	STR r1, [r0]
	
doSomethingUseful
	B doSomethingUseful
	   
GPIO_init ; r0 stores base address, r1 stores out val, r2 stores in val, r3 will store the or of the two
	ORR r3, r2, r1			; store active pins
	STR r1, [r0, #GPIODIR] 	; set directions
	STR r3, [r0, #GPIODR2R]	; set power (2mA)
	STR	r1, [r0, #GPIOODR]	; set outputs open drain
	STR r2, [r0, #GPIOPDR]	; set inputs to pull downs
	STR r3, [r0, #GPIODEN]	; enable pins
	BX lr
	
; r0 stores base address, r1 stores timeout val
GPTM_Edge_Count_init
	MOV r2, #0x00
	STR r2, [r0, #GPTMCTL]
	
	MOV r2, #0x04
	STR r2, [r0, #GPTMCFG]
	
	MOV r2, #0x03
	STR r2, [r0, #GPTMTAMR]
	
	STR r1, [r0, #GPTMTAILR]
	
	MOV r2, #0x02
	STR r2, [r0, #GPTMIMR]
	
	MOV r2, #0x01
	STR r2, [r0, #GPTMCTL]
	
	BX lr

Timer0A_Handler
	; clear interrupt
	LDR r0, GPTM0
	MOV r1, #0x01
	STR r1, [r0, #GPTMICR]
	
	; increment timeout
	LDR r0, TIMEOUTCOUNT
	LDR r1, [r0]
	ADD r1, #1
	
	; check state
	LDR r0, LIGHT_STATE
	LDR r2, [r0]
	
	; check if 20 seconds have passed
	MOV r3, #RED_STATE
	CMP r2, r3
	IT EQ
		MOVEQ r4, #4
		
	; check if 5 seconds have passed
	MOV r3, #YELLOW_STATE
	CMP r2, r3
	IT EQ
		MOVEQ r4, #1
		
	; check if 20 seconds have passed
	MOV r3, #GREEN_STATE
	CMP r2, r3
	IT EQ
		MOVEQ r4, #6
	
	; check timeout against stored compare value
	CMP r1, r4
	ITT EQ
		ADDEQ r2, #1 ; increment state
		MOVEQ r1, #0
		
	; store timeout count
	LDR r0, TIMEOUTCOUNT
	STR r1, [r0]
	
	; check state rollover
	CMP r2, #3
	IT EQ
		MOVEQ r2, #0
		
	; store state
	LDR r0, LIGHT_STATE
	STR r2, [r0]
		
	; change light state
	LDR r0, GPIOC
	
	CMP r2, #RED_STATE
	IT EQ
		MOVEQ r1, #RED_LIGHT
		
	CMP r2, #YELLOW_STATE
	IT EQ
		MOVEQ r1, #YELLOW_LIGHT
	
	CMP r2, #GREEN_STATE
	IT EQ
		MOVEQ r1, #GREEN_LIGHT
	
	STR r1, [r0, #GPIODATA]
	
	BX lr

       ALIGN      
       END  
           