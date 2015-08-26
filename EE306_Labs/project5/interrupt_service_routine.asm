.ORIG	x1500
;save registers
	ST	R0,SaveR0
	ST	R1,SaveR1
	ST	R2,SaveR2
	ST	R3,SaveR3
	ST	R4,SaveR4
	ST	R5,SaveR5
	ST	R7,SaveR7

;get character from keyboard
KEYB	LDI	R0,KBSR
	BRzp	KEYB		;loop until user has entered a key
	LDI	R1,KBDR		;save the ASCII code into R1

;make sure it is a capital letter
	LD	R2,ASCII1	;load R2 with negative ASCII code of A
	ADD	R3,R1,R2
	BRn	DONE		;if value is less than A, then not a valid input, so terminate
	LD	R2,ASCII2	;load R2 with negative ASCII code of Z
	ADD	R3,R1,R2
	BRp	DONE		;if value is greater than Z, then not a valid input, so terminate
	
;print onto the screen
	LD	R3,COUNTER	;set up R3 as a counter for the number of times to print
DISP	LDI	R0,DSR
	BRzp	DISP		;loop until monitor is ready
	STI	R1,DDR		;display the character onscreen
	ADD	R3,R3,#-1	;decrement the counter
	BRp	DISP

;convert to a lower case letter
	LD	R2,ASCIIL	;load R2 with x20
	ADD	R1,R1,R2	;convert ASCII code in R1 to lower case
	LD	R3,COUNTER	;set up R3 as a counter for the number of times to print

;print onto the screen
DISPL	LDI	R0,DSR
	BRzp	DISPL		;loop until monitor is ready
	STI	R1,DDR		;display the character onscreen
	ADD	R3,R3,#-1	;decrement the counter
	BRp	DISPL

;print a newline
	LD	R1,NEWLINE
NEWL	LDI	R0,DSR
	BRzp	NEWL		;loop until monitor is ready
	STI	R1,DDR		;display the newline
	BRnzp	DONE

;restore registers and terminate
DONE	LD	R0,SaveR0
	LD	R1,SaveR1
	LD	R2,SaveR2
	LD	R3,SaveR3
	LD	R4,SaveR4
	LD	R5,SaveR5
	LD	R7,SaveR7
	RTI

NEWLINE	.FILL x0A
COUNTER	.FILL #10
ASCII1	.FILL xFFBF
ASCII2	.FILL xFFA6
ASCIIL	.FILL x20
KBSR	.FILL xFE00
KBDR	.FILL xFE02
DSR	.FILL xFE04
DDR	.FILL xFE06
SaveR0	.BLKW 1
SaveR1	.BLKW 1
SaveR2	.BLKW 1
SaveR3	.BLKW 1
SaveR4	.BLKW 1
SaveR5	.BLKW 1
SaveR7	.BLKW 1

.END