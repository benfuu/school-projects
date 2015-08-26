;initialization
.ORIG X3000				;start program at x3000
		LD	R3,REQUEST	;load R3 with request string to be used as pointer
		LD	R5,ROOMS	;load R5 with the room state

;load the request
REQUESTROOM	LD	R5,ROOMS	;load R5 with room state
		LDR	R4,R3,#0	;load request to R4
		ADD	R4,R4,#0	;set R4 for branching
		BRz	FINISH		;if the request is x0000, branch to finish

;decode the start time
STARTTIME	LD	R6,MASK		;load R6 with the mask
		AND	R7,R7,#0	;clear R7 to be used as a counter
		ADD	R7,R7,#5	;set R7 to 5, the number of times to shift the bit left
TESTONE		ADD	R7,R7,#0	;set the condition codes for R7
		BRz	FINISHSHIFT	;finish shifting if R7 is 0
		ADD	R4,R4,#0	;set R4, the request
		BRzp	POSONE		;if high bit of R4 is 0, skip to positive shift
NEGONE		ADD	R4,R4,R4	;if high bit is 1, shift the bit to the left
		ADD	R4,R4,#1	;record a 1 to the right bit
		ADD	R7,R7,#-1	;decrement counter
		BRz	FINISHSHIFT	;finish shifting if R7 is 0
		ADD	R4,R4,#0	;set R4 for branching
		BRn	NEGONE		;branch back to negative if high bit was 1
POSONE		ADD	R4,R4,R4	;if high bit of R4 is 0, shift bit to the left
		ADD	R7,R7,#-1	;decrement counter
		BRnzp	TESTONE		;branch back to retest the bit
FINISHSHIFT	AND	R0,R4,R6	;place shifted result into R0

;reload R4
		LDR	R4,R3,#0	;load request to R4

;decode the end time
ENDTIME		LD	R6,MASK		;load R6 with the mask
		AND	R7,R7,#0	;clear R7 to be used as a counter
		ADD	R7,R7,#10	;set R7 to 10, the number of times to shift the bit left
TESTTWO		ADD	R7,R7,#0	;set the condition codes for R7
		BRz	FINISHSHIFTTWO	;finish shifting if R7 is 0
		ADD	R4,R4,#0	;set R4, the request
		BRzp	POSTWO		;if high bit of R4 is 0, skip to positive shift
NEGTWO		ADD	R4,R4,R4	;if high bit is 1, shift the bit to the left
		ADD	R4,R4,#1	;record a 1 to the right bit
		ADD	R7,R7,#-1	;decrement counter
		BRz	FINISHSHIFTTWO	;finish shifting if R7 is 0
		ADD	R4,R4,#0	;set R4 for branching
		BRn	NEGTWO		;branch back to negative if high bit was 1
POSTWO		ADD	R4,R4,R4	;if high bit of R4 is 0, shift bit to the left
		ADD	R7,R7,#-1	;decrement counter
		BRnzp	TESTTWO		;branch back to retest the bit
FINISHSHIFTTWO	AND	R1,R4,R6	;place shifted result into R1

;reload R4
		LDR	R4,R3,#0	;load request to R4

;decode user ID
		LD	R6,MASK		;load mask into R6
		AND	R2,R4,R6	;only consider the last 5 bits

;increment the time
		ADD	R6,R0,R5	;add the start time to R5 and store to temporary time R6
		NOT	R5,R0		;negate R0, store into R5
		ADD	R5,R5,#1	;form 2's complement negative
		ADD	R7,R1,R5	;subtract R5 from R1 and make counter R7
TESTREQ		LDR	R4,R6,#0	;load the room's time into R4
		BRz	EMPTY		;branch if room is empty
		NOT	R5,R2		;form negative of R2 and place into R5
		ADD	R5,R5,#1	;form 2's complement negative
		ADD	R5,R5,R4	;see if user ID is the same
		BRz	EMPTY		;branch since user ID is the same
		ADD	R3,R3,#1	;increment request
		BRnzp	REQUESTROOM	;get next request
EMPTY		ADD	R6,R6,#1	;increment the time slot
		ADD	R7,R7,#-1	;decrement counter
		BRz	WRITEROOM	;if counter hits 0, all rooms are valid so write user
		ADD	R7,R7,#0	;set R7 for branching
		BRnzp	TESTREQ		;since counter was not 0, branch back to test next time
WRITEROOM	LD	R5,ROOMS	;reload R5 with the room state
		ADD	R6,R0,R5	;add the start time to the room state
		NOT	R4,R0		;negate R0, store into R4
		ADD	R4,R4,#1	;form 2's complement negative
		ADD	R7,R1,R4	;subtract R4 from R1 and make counter R7
WRITEUSER	STR	R2,R6,#0	;store the user ID into R6
		ADD	R7,R7,#-1	;decrement counter
		BRz	DONE		;counter hit 0, so done writing users
		ADD	R6,R6,#1	;increment the room state and write the next room
		BRnzp	WRITEUSER	;write the next user ID
DONE		ADD	R3,R3,#1	;increment request
		BRnzp	REQUESTROOM

;finish
FINISH		ADD	R1,R1,#0
		


REQUEST .FILL x3200
ROOMS .FILL X3100
MASK .FILL x001F
.END
