.ORIG x3000
;fill memory locations with rocks
	LD	R1,Rock		;load R1 with ASCII for "o"
	LD	R0,RowAmem	;load R0 with x4000
	AND	R3,R3,#0	;clear R3 to be used as counter
	ADD	R3,R3,#3	;load R3 with #3, number of rocks in row A
RowAadd	STR	R1,R0,#0	;store "o"
	ADD	R0,R0,#1	;increment R0
	ADD	R3,R3,#-1	;decrement counter
	BRp	RowAadd		;repeat until x4001-x4003 are filled

	LD	R1,Rock		;load R1 with ASCII for "o"
	LD	R0,RowBmem	;load R0 with x4005
	AND	R3,R3,#0	;clear R3 to be used as counter
	ADD	R3,R3,#5	;load R3 with #5, number of rocks in row B
RowBadd	STR	R1,R0,#0	;store "o"
	ADD	R0,R0,#1	;increment R0
	ADD	R3,R3,#-1	;decrement counter
	BRp	RowBadd		;repeat until x4006-x400A are filled

	LD	R1,Rock		;load R1 with ASCII for "o"
	LD	R0,RowCmem	;load R0 with x400C
	AND	R3,R3,#0	;clear R3 to be used as counter
	ADD	R3,R3,#8	;load R3 with #8, number of rocks in row C
RowCadd	STR	R1,R0,#0	;store "o"
	ADD	R0,R0,#1	;increment R0
	ADD	R3,R3,#-1	;decrement counter
	BRp	RowCadd		;repeat until x400D-x4014 are filled

;set R1
AND	R1,R1,#0		;clear R1 to be used as a indicator for player (0 for player 1, 1 for player 2)
	
;display the state
Prompt	LD	R0,RowAmem	;load R0 with Row A starting address
	LDR	R3,R0,#0
	BRp	Display		;if it contains something, then game is not over, so play
	LD	R0,RowBmem	;load R0 with Row B starting address
	LDR	R3,R0,#0
	BRp	Display		;if it contains something, then game is not over, so play
	LD	R0,RowCmem	;load R0 with Row C starting address
	LDR	R3,R0,#0
	BRp	Display		;if it contains something, then game is not over, so play
	BRnzp	Over		;if all rows are empty, then branch to finish	
Display	LEA	R0,RowA		;load R0 with the first line Row A
	TRAP	x22		;display Row A on the screen
	LD	R0,RowAmem	;load R0 with Row A memory starting address containing rocks
	TRAP	x22		;display the o's on screen
	LD	R0,Enter	;load R0 with ASCII for enter
	TRAP	x21		;display the new line

	LEA	R0,RowB		;load R0 wih second line Row B
	TRAP	x22		;display Row B on the screen
	LD	R0,RowBmem	;load R0 with Row A memory starting address containing rocks
	TRAP	x22		;display the o's on screen
	LD	R0,Enter	;load R0 with ASCII for enter
	TRAP	x21		;display the new line

	LEA	R0,RowC		;load R0 with third line Row C
	TRAP	x22		;display Row C on the screen
	LD	R0,RowCmem	;load R0 with Row A memory starting address containing rocks
	TRAP	x22		;display the o's on screen
	LD	R0,Enter	;load R0 with ASCII for enter
	TRAP	x21		;display the new line

;display player's turn
	
TestP	ADD	R1,R1,#0	;set condition codes
	BRp	PL2		;branch to player 2's prompt

PL1	LEA	R0,Player1
	TRAP	x22		;display player 1's prompt
	BRnzp	Input		;get the next move
PL2	LEA	R0,Player2
	TRAP	x22		;display player 2's prompt
	BRnzp	Input		;get the next move


;get the user input
Input	JSR	SaveReg		;save registers
	TRAP	x20		;get the character and store to R0
	ADD	R1,R0,#0	;store the character to R1
	TRAP	x21		;display the character onscreen
	TRAP	x20		;get the next character
	ADD	R2,R0,#0	;store character to R2
	TRAP	x21		;display the character onscreen
	LD	R0,Enter	
	TRAP	x21		;display newline

;test for a valid move
	LD	R3,RowoffsetA
	LD	R4,RowoffsetB
	LD	R5,RowoffsetC
	ADD	R3,R1,R3	;see if character was "A" or not
	BRz	NextA		
	ADD	R4,R1,R4	;see if character was "B" or not
	BRz	NextB
	ADD	R5,R1,R5	;see if character was "C" or not
	BRz	NextC
	BRnzp	Retry

Testnum	LD	R3,Numoffset1
	LD	R4,Numoffset2
	ADD	R3,R2,R3	;see if character was less than "1"
	BRn	Retry		;display invalid prompt
	ADD	R4,R2,R4	;see if character was greater than "9"
	BRp	Retry		;display invalid prompt
	RET

NextA	JSR	Testnum
	LD	R3,NumASCII	;load R3 with hex to digit offset
	ADD	R2,R2,R3	;convert R2 digit to decimal value to use as count
	ADD	R4,R2,#0	;store temporary count in R4
	LD	R1,RowAmem	;load R1 with x4000, pointer to first in Row A
AgainA	LDR	R3,R1,#0	;load R3 with contents of x4000
	BRz	TestA		;if contents are null, continue
	ADD	R1,R1,#1	;increment R1
	ADD	R4,R4,#-1	;decrement the counter
	BRnzp	AgainA
TestA	ADD	R1,R1,#-1	;move R1 to last "rock" in row A
	ADD	R4,R4,#0	;test the counter to see if it's still positive
	BRp	Retry		;number was larger than # of rocks, so move is bad
	AND	R6,R6,#0	;move was good, so prepare R6 for storing
StoreA	STR	R6,R1,#0	;clear the contents of R1, clear the "rocks"
	ADD	R1,R1,#-1	;decrement R1
	ADD	R2,R2,#-1	;decrement the counter
	BRp	StoreA		;keep storing and decrementing
	BRnzp	Done
	

NextB	JSR	Testnum
	LD	R3,NumASCII	;load R3 with hex to digit offset
	ADD	R2,R2,R3	;convert R2 digit to decimal value
	ADD	R4,R2,#0	;store temporary count in R4
	LD	R1,RowBmem	;load R1 with x4005, pointer to first in Row B
AgainB	LDR	R3,R1,#0	;load R3 with contents of x4005
	BRz	TestB		;if contents are null, continue
	ADD	R1,R1,#1	;increment R1
	ADD	R4,R4,#-1	;decrement the counter
	BRnzp	AgainB
TestB	ADD	R1,R1,#-1	;move R1 to last "rock" in row B
	ADD	R4,R4,#0	;test the counter to see if it's still positive
	BRp	Retry		;number was larger than # of rocks, so move is bad
	AND	R6,R6,#0	;move was good, so prepare R6 for storing
StoreB	STR	R6,R1,#0	;clear the contents of R1, clear the "rocks"
	ADD	R1,R1,#-1	;decrement R1
	ADD	R2,R2,#-1	;decrement the counter
	BRp	StoreA		;keep storing and decrementing until R2 is 0
	BRnzp	Done

NextC	JSR	Testnum
	LD	R3,NumASCII	;load R3 with hex to digit offset
	ADD	R2,R2,R3	;convert R2 digit to decimal value
	ADD	R4,R2,#0	;store temporary count in R4
	LD	R1,RowCmem	;load R1 with x400C, pointer to first in Row C
AgainC	LDR	R3,R1,#0	;load R3 with contents of x400C
	BRz	TestC		;if contents are null, continue
	ADD	R1,R1,#1	;increment R1
	ADD	R4,R4,#-1	;decrement the counter
	BRnzp	AgainC
TestC	ADD	R1,R1,#-1	;move R1 to last "rock" in row C
	ADD	R4,R4,#0	;test the counter to see if it's still positive
	BRp	Retry		;number was larger than # of rocks, so move is bad
	AND	R6,R6,#0	;move was good, so prepare R6 for storing
StoreC	STR	R6,R1,#0	;clear the contents of R1, clear the "rocks"
	ADD	R1,R1,#-1	;decrement R1
	ADD	R2,R2,#-1	;decrement the counter
	BRp	StoreC		;keep storing and decrementing until R2 is 0
	BRnzp	Done

;restore registers and get next prompt
Done	JSR	RestoreReg	;restore registers
	ADD	R1,R1,#0	;set cc
	BRp	Play2		;if R1 is positive, then it was player 2's turn
	ADD	R1,R1,#1	;R1 is 0, so add 1 to go to player 2
	LD	R0,Enter
	TRAP	x21		;display a new line
	BRnzp	Prompt
Play2	AND	R1,R1,#0	;reset to Player 1
	LD	R0,Enter
	TRAP	x21		;display a new line	
	BRnzp	Prompt

;display invalid prompt message
Retry	LEA	R0,Invalid	
	TRAP	x22		;display invalid prompt onscreen
	LD	R0,Enter
	TRAP	x21		;display a new line
	JSR	RestoreReg	;restore registers
	BRnzp	TestP		;show the same prompt
	

Over	ADD	R1,R1,#0	;set cc
	BRp	TWOWin		;if positive, then player 1 won
	LEA	R0,Player1win	;0, so player 1 won
	TRAP	x22		;display the message on the screen
	BRnzp	Stop

TWOWin	LEA	R0,Player2win	;positive, so player 2 won
	TRAP	x22		;display the message on the screen
	BRnzp	Stop
Stop	TRAP 	x25		;halt machine: game over

;
SaveReg		ST	R1,SaveR1
		ST	R2,SaveR2
		ST	R3,SaveR3
		ST	R4,SaveR4
		ST	R5,SaveR5
		ST	R6,SaveR6
		RET
;
RestoreReg	LD	R1,SaveR1
		LD	R2,SaveR2
		LD	R3,SaveR3
		LD	R4,SaveR4
		LD	R5,SaveR5
		LD	R6,SaveR6
		RET

SaveR1	.FILL	x0000
SaveR2	.FILL	x0000
SaveR3	.FILL	x0000
SaveR4	.FILL	x0000
SaveR5	.FILL	x0000
SaveR6	.FILL	x0000

RowoffsetA	.FILL	xFFBF
RowoffsetB	.FILL	xFFBE
RowoffsetC	.FILL	xFFBD

Numoffset1	.FILL	xFFCF
Numoffset2	.FILL	xFFC7

NumASCII	.FILL	#-48

Rock	.FILL	x6F
Space	.FILL	x20
Enter	.FILL	x0A
Os	.FILL	x4000		;location for the rocks
RowAmem	.FILL	x4000		;starting location for memory locations for row A
RowBmem	.FILL	x4005		;starting location for memory locations for row B
RowCmem	.FILL	x400C		;starting location for memory locations for row C
RowA	.STRINGZ "ROW A: "
RowB	.STRINGZ "ROW B: "
RowC	.STRINGZ "ROW C: "
Player1	.STRINGZ "Player 1, choose a row and number of rocks: "
Player2	.STRINGZ "Player 2, choose a row and number of rocks: "
Player1win	.STRINGZ "Player 1 Wins."
Player2win	.STRINGZ "Player 2 Wins."
Invalid	.STRINGZ "Invalid move. Try again."

;rocks' memory should look like:
;x4000: Row A, x4001-x4003: rocks(1), x4004: x0000
;x4005:	Row B, x4006-x400A: rocks(5), x400B: x0000
;x400C: Row C, x400D-x4014: rocks(8), x4015: x0000

.END