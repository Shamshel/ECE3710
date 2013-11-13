
       THUMB
       AREA    DATA, ALIGN=2
       ALIGN          
       AREA    |.text|, CODE, READONLY, ALIGN=2
       EXPORT  Start
	   
; r0 and r1 store source addresses, r2 stores destination address
; assume little endian (MSW is at address+12)
Start
	; MOV r0, #0x
	; clear carry bit (and r3)
	MOV r3, #0x00
	CMP r3, #0x01
    B   ADD128

; r3 stores count
ADD128
	LDR r4, [r0, r3]
	LDR r5, [r1, r3]
	IT HI
		ADDHI r4, #0x01
		
	ADD r6, r4, r5
	STR r6, [r0, r3]
	
	ADD r3, #4
	
	CMP r3, #12
	BNE Fin
	
	B ADD128

Fin
	B Fin
       ALIGN      
       END  
           