			; Vector Table
			.ORIG x0200
			.FILL x0000		; Empty
			.FILL x1200		; Interrupt Service Routine
			.FILL x1600		; Protection Exception Handler
			.FILL x1A00		; Unaligned Access Exception Handler
			.FILL x1C00		; Unknown Opcode Exception Handler
			.END