.ORIG X3000
;get input from keyboard
Input	LEA	R0,Prompt
	TRAP x22		;print "Type a student ID and press Enter:"
	TRAP x20		;get character
	TRAP x21		;output character onscreen
	ADD	R2,R0,#0	;store R0 to R2 for later testing
	LD	R1,Done		;load R1 with -100
	ADD	R0,R0,R1	;see if R0 is 0
	BRz	FINISH		;branch to FINISH if R0 is 0	
	TRAP x20		;get the enter key
	TRAP x21		;output the enter key
	LD	R1,ASCII	;load R1 with ASCII inverse
	ADD	R0,R2,R1	;convert R1 to binary and store the value in R0

;test character
	LD	R1,Start	;load R1 with x3300, the first node
	LDR	R1,R1,#0	;load R1 with the location of first node
	BRnp	TEST		;test the character if R1 is a valid node
	LEA	R0,Noentry	;load R0 with the ASCII "No Entry"
	TRAP x22		;display "No Entry" onscreen
	LD	R0,Enter	;load R0 with the "Enter" ASCII
	TRAP x21		;display an Enter
	BRnzp	Input		;get next character
TEST	LDR	R2,R1,#2	;load R2 with the Student ID
	NOT	R3,R2		;negate R2 and store into R3
	ADD	R3,R3,#1	;form 2's complement of the negative
	ADD	R3,R0,R3	;subtract R2 from R0
	BRp	Pos		;R0 is bigger, so use right child
	BRn	Neg		;R0 is smaller, so use left child
	LDR	R0,R1,#3	;load R0 with ASCII pointer
	TRAP x22		;display result onscreen	
	LD	R0,Enter	;load R0 with the "Enter" ASCII
	TRAP x21		;display an Enter
	BRnzp	Input		;get next character
Pos	LDR	R1,R1,#1	;load R1 with the next node
	BRnp	TEST		;test the ID if contents are not null
	LEA	R0,Noentry	;load R0 with the ASCII "No Entry"
	TRAP x22		;display "No Entry" onscreen
	LD	R0,Enter	;load R0 with the "Enter" ASCII
	TRAP x21		;display an Enter
	BRnzp	Input		;get next character
Neg	LDR	R1,R1,#0	;load R1 with the next node
	BRnp	TEST		;test the ID if contents are not null
	LEA	R0,Noentry	;load R0 with the ASCII "No Entry"
	TRAP x22		;display "No Entry" onscreen
	LD	R0,Enter	;load R0 with the "Enter" ASCII
	TRAP x21		;display an Enter
	BRnzp	Input		;get next character
FINISH	TRAP x20		;get enter key
	LD	R0,Enter	;load R0 with the "Enter" ASCII
	TRAP x21
	BRnzp	EXIT		;exit and halt program	
EXIT	TRAP x25

Start	.FILL	x3300
Enter	.FILL	x0A
Done	.FILL	#-100
ASCII	.FILL	#-48
Prompt	.STRINGZ "Type a student ID and press Enter:"
Noentry	.STRINGZ "No Entry"
	.END