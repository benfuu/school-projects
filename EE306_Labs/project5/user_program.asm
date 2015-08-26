.ORIG	x3000
	LD	R6,STACK	;initialize the stack pointer
	LD	R0,INT		;load R0 with address of interrupt routine
	STI	R0,IVT		;store the address into the interrupt vector table
;enable IE
	LDI	R1,KBSR		;load R1 with the keyboard status register
	LD	R2,MASK		;load R2 with the mask x4000
	NOT	R1,R1
	NOT	R2,R2
	AND	R1,R1,R2
	NOT	R1,R1		;OR R1 with R2 so R1 contains KBSR with bit 14 set
	STI	R1,KBSR		;restore KBSR with bit 14 set
;print Texas
PRINT	LEA	R0,TEXAS1	
	PUTS			;print the first line of Texas onscreen
	LD	R0,NEWLINE
	OUT			;print a line feed
	JSR	DELAY
	LEA	R0,TEXAS2
	PUTS			;print the second line of Texas
	LD	R0,NEWLINE
	OUT			;print a line feed
	JSR	DELAY
	BRnzp	PRINT
	
	HALT

DELAY   ST  R1, SaveR1
        LD  R1, COUNT
REP     ADD R1,R1,#-1
        BRp REP
        LD  R1, SaveR1
        RET
COUNT   .FILL #25000
SaveR1  .BLKW 1
	



STACK	.FILL x3000
IVT	.FILL x0180
INT	.FILL x1500
TEXAS1	.STRINGZ	"Texas       Texas       Texas       Texas"
TEXAS2	.STRINGZ	"       Texas       Texas       Texas"
KBSR	.FILL xFE00
MASK	.FILL x4000
NEWLINE	.FILL x0A

.END