			; Unaligned Access Exception Handler
			.ORIG x1A00
			TRAP x25		; we just halt the machine
			.END