			; Timer Interrupt Service Routine
			; We'll be using R0 and R1, so make sure to save those registers.

			.ORIG x1200
			
			; Push R0 and R1 onto the stack
			STW R0, R6, #-1		; M[R6-2] = R0
			STW R1, R6, #-2		; M[R6-4] = R1

			; Increment M[x4000]
			LEA R0, COUNT		; R0 = COUNT
			LDW R0, R0, #0		; R0 = M[COUNT] = x4000
			LDW R1, R0, #0		; R1 = M[R0]
			ADD R1, R1, #1		; R1++
			STW	R1, R0, #0		; M[R0] = R1++

			; Pop R1 and R0 off of the stack
			LDW R1, R6, #-2		; R1 = M[R6-4]
			LDW R0, R6, #-1		; R0 = M[R6-2]

			RTI
COUNT		.FILL x4000
			.END