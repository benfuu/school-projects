		; sample program to sort a non-zero array from smallest to largest
		.ORIG x3000
		AND R7, R7, #0		; clear R7
		ADD R7, R7, #1		; just put a nonzero value into R7
OUTERLOOP	ADD R7, R7, #0		; if R7 =  0, then we're done because no swaps happened
		BRz DONE
		AND R7, R7, #0		; reset R7 to 0
		LD R0, ARRAY		; reload R0

		; let R1 point to first element, R2 point to second element
INNERLOOP	LDR R1, R0, #0
		BRz OUTERLOOP		; guard just in case array length is 0
		LDR R2, R0, #1
		BRz OUTERLOOP
		NOT R2, R2		
		ADD R2, R2, #1
		ADD R2, R1, R2		; do R1 - R2
		BRp SWAP		; need to swap if R2 > R1

DONESWAP	ADD R0, R0, #1		; increment pointer
		BR INNERLOOP

		; swaps M[R0] <-> M[R0+1]
SWAP		LDR R3, R0, #0		; TMP = M[R0]
		LDR R4, R0, #1		; TMP1 = M[R0+1]
		STR R4, R0, #0		; M[R0] = TMP1 = M[R0+1]
		STR R3, R0, #1		; M[R0+1] = TMP = M[R0]
		ADD R7, R7, #1		; register that we did a swap
		BR DONESWAP

DONE		TRAP x25
ARRAY		.FILL x4000
		.END