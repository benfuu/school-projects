			; User program.
			; Initializes M[x4000] to 1
			; Finds sum of first 20 bytes in memory starting at xC000 and stores sum into xC014

			.ORIG x3000

			; Initialize registers to 0
			AND R0, R0, #0
			AND R1, R1, #0
			AND R2, R2, #0
			AND R3, R3, #0
			AND R4, R4, #0
			AND R5, R5, #0

			; Set M[x4000] = 1
			;LEA R0, INIT
			;LDW R0, R0, #0		; R0 = x4000
			;ADD R1, R1, #1		; R1 = 1
			;STW R1, R0, #0		; M[R0] = R1
			
			; Find sum. We use R5 for the count and R4 for the sum
			LEA R0, ARRAY
			LDW R0, R0, #0		; R0 = xC000
			ADD R5, R5, #15
			ADD R5, R5, #5		; R5 = 20
			
FINDSUM		LDB R1, R0, #0		; R1 = M[R0]
			ADD R4, R4, R1		; SUM = SUM + R1
			ADD R0, R0, #1		; increment position in array
			ADD R5, R5, #-1		; R5--
			BRz DONE			; if R5 == 0, we are done
			BR FINDSUM

DONE		STW R4, R0, #0		; first store into correct location
			JMP R4
			;LEA R0, EXCPROT
			;LDW R0, R0, #0		; R0 = address to store result
			;STW R4, R0, #0		; M[R0] = SUM
			TRAP x25

INIT		.FILL x4000
ARRAY		.FILL xC000
RESULT		.FILL xC014
EXCPROT		.FILL x0000
EXCUNALIGN	.FILL xC017
			.END