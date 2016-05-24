; Solution to problem #6 on HW 8
; PUTPS - Print a packed string to the console.
; Input - R0 contains the address of the first memory location occupied by the string

		.ORIG x04E0		; address of PUTPS

		LEA R0, HELLO		; use this to test program

		; Save registers we use except for R0 and R1.
		LEA R1, SAVEREG
		STR R2, R1, #2
		STR R3, R1, #3
		STR R4, R1, #4
		STR R7, R1, #7		; we'll be calling another subroutine so we must save R7

		; Mask the first character.
LOOP		LDR R1, R0, #0
		LD R2, MASK_FIRST	; mask the least significant 8 bits
		AND R1, R1, R2
		BRz DONE		; make sure it's not the sentinel
		JSR DISPLAY
		
		; Mask the second character.
		LDR R1, R0, #0
		LD R2, MASK_SECOND	; mask the most significant 8 bits
		AND R1, R1, R2
		BRz DONE		; make sure it's not the sentinel
		JSR SHIFT_RIGHT
		JSR DISPLAY
		ADD R0, R0, #1		; increment our pointer
		BR LOOP


		; Shifts R1 8 bits to the right
SHIFT_RIGHT	AND R2, R2, #0
		ADD R2, R2, #8
SHIFT_LOOP	ADD R1, R1, #0		; check if most significant bit is a 1
		BRn ADD_1
		ADD R1, R1, R1
		ADD R2, R2, #-1
		BRz DONE_SHIFT
		BR SHIFT_LOOP

ADD_1		ADD R1, R1, R1
		ADD R1, R1, #1
		ADD R2, R2, #-1
		BRz DONE_SHIFT
		BR SHIFT_LOOP

DONE_SHIFT	RET


		; Print the character in R1 into the console.
DISPLAY		LDI R2, DSR
		BRzp DISPLAY
		STI R1, DDR
		RET


		; Restore registers and return.
DONE		LEA R1, SAVEREG
		LDR R2, R1, #2
		LDR R3, R1, #3
		LDR R4, R1, #4
		LDR R7, R1, #7
		RET

MASK_FIRST	.FILL x00FF
MASK_SECOND	.FILL xFF00
DSR		.FILL xFE04
DDR		.FILL xFE06
SAVEREG		.BLKW 8
HELLO		.FILL x6548	; use this to test program
		.FILL x6c6c
		.FILL x006F
		.END