			; Timer Interrupt Service Routine
			; We'll be using R0 and R1, so make sure to save those registers.

			.ORIG x1200
			
			; Push R0 and R1 onto the stack
			STW R0, R6, #-1		; M[R6-2] = R0
			STW R1, R6, #-2		; M[R6-4] = R1

			; Clear reference bit in each PTE in the Page Table
			LEA R0, PAGETABLE	; R0 = PAGETABLE
			LDW R0, R0, #0		; R0 = M[PAGETABLE] = x1000
LOOP		LDW R1, R0, #0		; R1 = M[R0]
			AND R1, R1, #-2		; To clear bit 0, AND with 0xFFFE = -2
			STW	R1, R0, #0		; M[R0] = R1++
			LEA R1, LR			; R1 = LR
			LDW R1, R1, #0		; R1 = M[LR] = 0x10FE (last PTE)
			XOR R1, R1, R0		; R1 = R1 XOR R0
			BRz DONE			; if R0 == R1 (0x10FE), we are done
			ADD R0, R0, #2		; R0++
			BR LOOP

			; Increment M[x4000]
INCREMENT	LEA R0, COUNT		; R0 = COUNT
			LDW R0, R0, #0		; R0 = M[COUNT] = x4000
			LDW R1, R0, #0		; R1 = M[R0]
			ADD R1, R1, #1		; R1++
			STW	R1, R0, #0		; M[R0] = R1++

			; Pop R1 and R0 off of the stack
DONE		LDW R1, R6, #-2		; R1 = M[R6-4]
			LDW R0, R6, #-1		; R0 = M[R6-2]

			RTI
PAGETABLE	.FILL x1000
LR			.FILL x10FE
COUNT		.FILL x4000
			.END