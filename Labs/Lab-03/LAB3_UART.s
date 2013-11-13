; LAB3_UART.s
;
;

       THUMB
       AREA    DATA, ALIGN=2
       ALIGN          
       AREA    |.text|, CODE, READONLY, ALIGN=2
       EXPORT  Start
	   
RCGC1 EQU 0x400FE104 ;RCGC1 CTL
RCGC2 EQU 0x400FE108 ;RCGC2 address (to activate GPIO)
UART2 EQU 0x4000E000 ;base UART2 address
PORTD EQU 0x40007000 ;base GPIO_PortD address
PORTF EQU 0x40025000 ;base GPIO_PortF address
TIMER0 EQU 0x40030000 ;Timer0 base for random timer

message DCB "You are DOOMED!                          "
		DCB "Concentrate, you fool!                   "
		DCB "Nope...                                  "
		DCB "What a rubbish question                  "
		DCB "Only in your dreams                      "
		DCB "Yes, now leave me alone!                 "
		DCB "Heh, you wish...                         "
		DCB "Oh yeah that will happen                 "
		DCB "Stop bothering me!                       "
		DCB "Not if you were the last person on earth!"
		DCB "Suck it up, princess!                    "
		DCB "Cry me a river. Then drown.              "
		;DCB "No body likes you. No one.               "
		;DCB "When you were born, your mother cried!   "
		;DCB "No. Do the world a favor and die. Slowly."
		;DCB "I don't know... Go ask Gerdes.           "
		;DCB "When pigs fly and unicorns sing.         "
		;DCB "Your face!                               "
		;DCB "Your mom...                              "
	   
Start  
;1. enable the clock for UART2 in RCGC1 (bit 2)
	;Enable RCGC1 for UART2:
	LDR R1, =RCGC1
	MOV32 R0, #0x10004 ;bit 2 is set for UART2
	STR R0, [R1]
	nop
	nop
;2. enable RCGC2 for port D (and port F for the push button)
	;Enable RCGC2 for PortD:
	LDR R1, =RCGC2
	MOV R0, #0x28 ;0b00101000
	STR R0, [R1]
	nop
	nop
;3. Configure Port D alt. funct. and enable pins (P6 is Rx, P7 is Tx):
  ;a. enable alternate functionality for PD6 and PD7
	LDR R1, =PORTD
  ;b. Unlock pin 7
	MOV32 R0, #0x4C4F434B
	STR R0, [R1,#0x520] ;offset #0x520 is the LOCK register
  ;c. Commit the unlock
	MOV R0, #0xFF
	STR R0, [R1,#0x524] ;offset #0x524 is the CR register
  ;d. AFSEL on for P6 & P7
	MOV R0, #0xC0 ;0b11000000
	STR R0, [R1,#0x420] ;AFSEL_R
  ;e. configure alternate functionality
	MOV32 R0, #0x11000000 ;pins 6 and 7 are UART2 Rx and Tx respectively
	STR R0, [R1,#0x52C] ;offset 0x520 is the GPIOPCTL - specifies which alt. func. you want to use. See Table 23-5 in the data sheet
  ;f. enable Port D pins (P6 & P7)
	MOV R0, #0xC0
	STR R0, [R1,#0x51C]
;4. disable UART2 (CTL Register is offset #0x30 - bit 0 is UARTEN)
	LDR R1, =UART2
	MOV R0, #0x0 ;turn off the UARTEN bit
	STR R0, [R1,#0x30]
;5. set the baud rate divisor: BRD = SysClk/(16*baudrate) = 16MHz/(16*9600) = 104.1666667
	;I(nteger) = 104 = #0x68 (put in offset 0x24)
	MOV R0, #0x68
	STR R0, [R1,#0x24]
	;F(ractional) = (0.1666667*2^6+0.5) = 11 = #0xB (put in offset 0x28)
	MOV R0, #0xB
	STR R0, [R1,#0x28]
;6. Set serial parameters... (UARTLCTRH, line control, is #0x2C)
	;Parity on (CTRH, offset #0x2C, bit 2, 1 is on)
	;word length to 8 (offset 0x2C, bits 5 and 6 need to be 1, so 0b01100000 or 0x60)
	;FIFO buffers? (offset 0x2C, bit 4 - 0 disables buffers, 1 enables buffers)
	MOV R0, #0x60 ;0b0110.0010
	STR R0, [R1,#0x2C]
;7. Enable Tx/Rx and UART(offset 0x30 - bits 9, 8, and 0 set to 1, so #0x301)
	MOV R0, #0x301
	STR R0, [R1,#0x30]
;Configure PortF P4 to be our input button
	LDR R1, =PORTF
	;Enable DIR (offset 0x400)
	MOV R0, #0x0
	STR R0, [R1,#0x400]
	;Turn off AFSEL (offset 0x420)
	MOV R0, #0x0
	STR R0, [R1,#0x420]
	;Pull-up config (offset 0x510)
	MOV R0, #0x10
	STR R0, [R1,#0x510]
	;Enable pin 4 (offset 0x51C)
	STR R0, [R1,#0x51C]
;Configure TIMER0:
	LDR R1, =TIMER0
;Disable timer (TnEN should be cleared in GPTMCTL register, offset 0x00C)
	MOV R0, #0x0
	STR R0, [R1, #0xC] ;store a 0 into the CTL register to disable TIMER0
	;Cofigure the clock to 32-bit clock (CFG register is offset 0x0)
	MOV R0, #0x0 ;0x0 will configure as 32-bit
	STR R0, [R1, #0x0] ;offset 0x0 is the CFG register
	;Configure the clock as a period timer (TAMR is offset 0x4
	MOV R0, #0x2 ;0x2 will configure the timer as periodic
	STR R0, [R1, #0x4] ;offset 0x4 is the TAMR register
	;load in the start value into TAILR (offset 0x28)
	MOV R0, #0xB ;This is the number of messages we have in our 'Spiteful 8-ball'
	STR R0, [R1, #0x28]
	;Enable clock and start counting (by setting the TAEN bit in CTL - offset 0xC):
	MOV R0, #0x1 ;just enable the clock
	STR R0, [R1, #0xC]
	;to read the current value of GPTM TIMER0 TimerA, check the TAR register (offset 0x48)
	
resetLoop
	;send carriage return and line feed via UART
	LDR R1, =UART2
	MOV R0, #0xA ;ASCII for 'New Line, or Line Feed'
	STRB R0, [R1,#0x0]
	BL wait4emptyTx ;wait for character to transmit
	;send it
	LDR R1, =UART2
	MOV R0, #0xD ;ASCII for 'Carriage Return'
	STRB R0, [R1,#0x0] ;offset 0x0 is data
	BL wait4emptyTx ;wait for character to transmit
wait4input
	B sendMessage
	;check button
	LDR R1, =PORTF
	LDR R0, [R1,#0x3FC] ;load in button data
	ANDS R0, #0x10 ;check the button (PF.4)
	BEQ buttonRelease
	;check for input in UART
	LDR R1, =UART2
	LDR R0, [R1, #0x18] ;offset #0x018 bit 6 is the RxFF bit
	LDRB R2, [R1, #0x0] ;load in the data to clear RxFF bit
	;STR R2, [R1,#0x0]
	;BL wait4emptyTx
	ANDS R0, #0x40
	BNE sendMessage
	;if neither is pressed, B wait4input
	B wait4input

buttonRelease
	LDR R1, =PORTF
	LDR R0, [R1,#0x3FC] ;load in button data
	ANDS R0, #0x10 ;check the button (PF.4)
	BEQ buttonRelease
	B sendMessage

sendMessage
	;choose response
	LDR R1, =TIMER0
	LDR R0, [R1,#0x50] ;load Timer0 data
	MOV R6, #41 ;(each message is 41 bytes long)
	MUL R7, R0, R6 ;R1 = message number * 41 = message offset
	;send response via UART
sendChar
	LDR R0, =message
	LDRB R1, [R0,R7] ;R1 = current char
	ADD R7, #1 ;incrment message memory offset for next time through the loop
	LDR R0, =UART2
	STRB R1, [R0,#0x0]
	BL wait4emptyTx
	;check to see if the whole message has been sent
	SUBS R6, #1
	BNE sendChar ;if the solution is 0, z=1, don't loop back
	B resetLoop
	
wait4emptyTx
	;check TxFE
	LDR R1, =UART2
	LDR R0, [R1, #0x18]
	ANDS R0, #0x80 ;check the TxFE bit, bit 7
	BEQ wait4emptyTx ;if TxFE is 0, B wait4emptyTx
	BX LR



    ALIGN      
    END  