
       THUMB
       AREA    DATA, ALIGN=2
       ALIGN          
       AREA    |.text|, CODE, READONLY, ALIGN=2
       EXPORT  Start
	   
SYSCTL	DCD	0x400FE000
UART0	DCD	0x4000C000
GPIOA	DCD 0x40004000

RCCMASK DCD 0xF83FD7FF

RCC		EQU	0x060
RCC2	EQU	0x070
RCGCUART	EQU 0x104
RCGCGPIO	EQU	0x108

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
GPIOPUR		EQU 0x510
GPIOPDR		EQU	0x514
GPIODEN		EQU	0x51C
GPIOLOCK	EQU	0x520
GPIOCR		EQU 0x524
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
	; set system clock to 15.38 MHz
	; table 5-5 says BYPASS=0, SYSDIV=0xC
	LDR r0, SYSCTL
	LDR r1, [r0, #RCC] ; read
	LDR r2, RCCMASK
	AND r1, r1, r2
	ORR r1, #0x6400000 ; modify
	STR r1, [r0, #RCC] ; write
	nop
	nop
	
	; enable clocking
	MOV r1, #0x01
	STR r1, [r0, #RCGCGPIO]
	STR r1, [r0, #RCGCUART]
	
	; init GPIOA
	LDR r0, GPIOA
	MOV r1, #0x3 ; UART0 shares pins PA0:1
	STR r1, [r0, #GPIOAFSEL]
	STR r1, [r0, #GPIODEN]
	
	; init UART0
	; setup for 9600 baud
	LDR r0, UART0
	MOV r1, #0x0
	STR r1, [r0, #UARTCTL]
	; set BRD
	; BRD=15.38E8/(16*9600)=100.13
	; IBRD = 100
	MOV r1, #100
	STR r1, [r0, #UARTIBRD]
	; IBRD = int(0.13*2^6+0.5) = 8
	MOV r1, #8
	STR r1, [r0, #UARTFBRD]
	; 8 bit words, no fifo, no parity
	; 0b01100000 = 0x60
	MOV r1, #0x060
	STR r1, [r0, #UARTLCRH]
	; reactivate uart
	; enable rx, tx, and enable
	MOV r1, #0x301
	STR r1, [r0, #UARTCTL]
	nop
	nop
	nop
  
	LDR r0, UART0
  
	B getStream
	
getStream
	BL getData
	
	; first byte is number of bytes to follow
	MOV r2, r1
	MOV r3, #0x0
	MOV r4, #0x0
	B pushIncoming

; r2 stores the number of times to iterate, r3 stores the temp value, r4 stores the bytecount
pushIncoming
	CMP r4, #3
	ITT EQ
		PUSHEQ {r3}
		MOVEQ r3, #0x0
	
	; check done
	CMP r2, #0
	BEQ cleanup
	
	BL getData
	LSL r3, #8
	ADD r3, r3, r1
	ADD r4, #1
	SUB r2, #1
	
getData
	; wait for data
	; check for RXFF=1
	LDR r1, [r0, #UARTFR]
	ANDS r1, #0x40
	BEQ getData

	LDRB r1, [r0]

	BX lr
	
cleanup
	MOV r5, #4
	SUB r4, r5, r4
	CMP r4, #4
	BEQ Fin
	
	MOV r5, #8
	CMP r4, #0
	MUL r4, r4, r5
	LSL r3, r4
	PUSH {r3}
	
	B Fin

Fin
	B Fin
       ALIGN      
       END  
           