	; Compares the numbers in R1 and R2. The result of the comparison is put into R0 as follows:
	;	R0 = 0 if R1 == R2
	;	R0 = 1 if R1 > R2
	;	R0 = -1 if R1 < R2
	;
	; Note that this is just a snippet of a real program since R1 and R2 should have values in them.

	.ORIG x3000
	AND R0, R0, #0	; clear R0
	NOT R2, R2	; negate R2
	ADD R2, R2, #1
	ADD R3, R1, R2	; R3 = R1 - R2
	BRn LESS	; if R3 < 0, then R1 < R2 so we add -1 to R0
	BRp GREATER	; if R3 > 0, then R1 > R2 so we add 1 to R0
	BR DONE		; if R3 == 0, then R1 == R2 and we don't need to change R0

LESS	ADD R0, R0, #-1
	BR DONE

GREATER	ADD R0, R0, #1
	BR DONE		; this is unnecessary but here in case programmer adds code below this line

DONE	TRAP x25
	.END