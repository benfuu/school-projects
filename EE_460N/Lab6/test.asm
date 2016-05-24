		.ORIG x3000
		ADD R0, R0, #1
		LEA R4, A
		JSRR R4
		ADD R2, R1, #5
		ADD R3, R1, #5
		BRnz A
		TRAP x25
A		ADD R0, R0, #3
		ADD R1, R0, #2
		RET
		.END