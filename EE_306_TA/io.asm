	.ORIG x3000

	AND R2, R2, #0
	ADD R2, R2, #5
	LEA R1, ID

AGAIN	GETC
	OUT
	STR R0, R1, #0	; store into ID
	ADD R1, R1, #1	; increment pointer
	ADD R2, R2, #-1	; decrement counter
	BRp AGAIN

	AND R0, R0, #0	; write newline to console
	ADD R0, R0, xA
	OUT
	AND R2, R2, #0	; write 0 (sentinel) to string
	STR R2, R1, #0
	LEA R0, ID
	PUTS		; print the saved id to the console
	HALT

ID	.BLKW 6
	.END