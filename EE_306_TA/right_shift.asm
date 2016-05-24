;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Simple program that unpacks an ASCII string (2 characters).				;
;												;
;	The program loads an ASCII string (2 ASCII characters) from x4000 that are packed, 	;
;	masks out bits [16:8] and [7:0], and then shifts the character in bits [16:8] to the 	;
;	right by 8 bits.									;
;												;
;	The resulting unpacked ASCII characters are stored are stored into x4001 and x4002.	;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		.ORIG x3000		; tells the assembler that the program starts at x3000

		; Clear registers
		AND R0, R0, #0
		AND R1, R1, #0
		AND R2, R2, #0
		AND R3, R3, #0
		AND R4, R4, #0
		AND R5, R5, #0
		AND R6, R6, #0
		AND R7, R7, #0

		; Load MASK [7:0] into R0 and MASK [15:8] into R1
		LD R0, MASK_7_0		; R0 = M[MASK_7_0] = x00FF
		LD R1, MASK_15_8	; R1 = M[MASK_15_8] = xFF00

		LD R2, PACKED		; R2 = M[PACKED] = X4000
	
		; Load packed ASCII string into R3
		LDR R3, R2, #0		; R3 = M[X4000] = ASCII String (Packed)
	
		; Separate the string by its most and least significant characters
		AND R4, R3, R0		; R4 = ASCII[7:0]
		AND R5, R3, R1		; R5 = ASCII[15:8]

		; Our function to shift R5 to the right by 8 bits.
		; Note that shifting a bit sequence 8 bits to the right is the same as
		; rotating the bit sequence 16-8 bits to the left.
		; At the end of execution, R5 = ASCII[15:8] >> 8

		ADD R6, R6, #8		; Let R6 be our counter to keep track of the number of rotates
	
START		ADD R6, R6, #-1		; Decrement the counter first (pre-decrement)
		BRN DONE		; We are finished if the counter < 0
		ADD R5, R5, R5		; Shift left by 1
		BRN NEG			; Check if the most significant bit is 1
		BR START		; Loop until done

		; If the most significant bit is 1, we shift left by 1 again and add 1 to the register to simulate a rotate.
NEG		ADD R5, R5, R5		; Shift left by 1
		ADD R5, R5, #1		; Make the least significant bit a 1
		ADD R6, R6, #-1		; Decrement the counter again (since we shifted left again)
		BRN DONE		; We are finished if the counter < 0
		BR START		; Loop back to START

		; Store the resulting unpacked characters into x4001 and x4002
DONE		LD R7, UNPACKED
		AND R5, R5, R0		; Mask out bits 15 to 8
		STR R5, R7, #0
		AND R4, R4, R0		; Mask out bits 15 to 8
		STR R4, R7, #1

		TRAP X25

MASK_7_0	.FILL	x00FF
MASK_15_8	.FILL	xFF00
PACKED		.FILL	x4000
UNPACKED	.FILL	x4001
		.END