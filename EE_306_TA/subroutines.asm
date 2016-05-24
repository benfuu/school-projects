;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;													;	
;	Sample program to demonstrate proper use of subroutines.					;
;													;
;	Note the difference between TRAP routines and normal subroutines (JSR(R)).			;
;													;
;	TRAP routines are located in system memory. The TRAP vector table is located in memory from	;
;	0x0000 to 0x00FF. A sample snapshot of what the TRAP vector table looks like can be found in	;
;	your textbook in section 9.1.2. For example, the value of memory location 0x0025 is 0xFD70.	;
;	This means that the service routine instructions to halt the machine (TRAP x25) are located	;
;	starting at memory location 0xFD70. The full TRAP x25 subroutine is given in Figure 9.6.	;
;													;
;	Subroutines are usually a set of instructions located in your program that you can call. The	;
;	main advantage of using subroutines is the ability to simplify your code's logic. For example,	;
;	if you need to perform the same function over and over again (such as sorting a list or		;
;	computing a value from multiple registers) it is much more effective to use a subroutine and	;
;	just call that multiple times rather than writing redundant code.				;
;													;
;	Remember the most important aspects of subroutines we discussed in recitation:			;
;	- Saving the incremented PC in R7 before loading the PC with the address of the subroutine	;
;	- Saving registers using caller/callee save							;
;	- Calling RET to restore the PC from R7 (same as JMP R7)					;
;													;
;	When TRAP or JSR(R) is called, the incemented PC is saved into R7 before the PC is loaded:	;
;		R7 <- PC++ (incremented PC from FETCH)							;
;		PC <- Address of TRAP routine or subroutine						;
;													;
;	When you call RET (JMP R7), the PC is restored from R7 and your program resumes:		;
;		PC <- R7										;
;													;
;	The basic layout of a subroutine should be this:						;
;													;			
;			JSR SUBROUTINE	; you call this from your main program				;
;													;
;	SUBROUTINE	ST R0, SAVER0	; callee save - save any registers that will be changed		;
;													;
;			ADD R0, R0, R0	; your subroutine code goes here				;
;													;
;			...										;
;													;
;			LD R0, SAVER0	; restore the registers						;
;			RET		; return from subroutine					;
;													;
;			...		; program code							;
;													;
;	SAVER0		.BLKW 1		; reserve 1 word of memory for saving a register		;
;													;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;													;
;	This sample program simply reverses the contents of an array. For example, for the array:	;
;													;													;
;	[0, 1, 2, 3, 4, 5]										;
;													;
;	The program will reverse the contents to look like:						;
;													;
;	[5, 4, 3, 2, 1, 0]										;
;													;
;	The array is located at memory location 0x5000 and is terminated by the sentinel #-1 (0xFFFF).	;
;	So in reality, the arrays would look like:							;
;													;
;	[0, 1, 2, 3, 4, 5, xFFFF]									;
;													;
;	And then after execution:									;
;													;
;	[5, 4, 3, 2, 1, 0, xFFFF]									;
;													;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


		.ORIG x3000

		; Let's have two register pointers:
		; - R0 will point to the beginning of the array
		; - R1 will point to the end of the array

		LD R2, SENTINEL	; R2 = 0xFFFF = #-1
		NOT R2, R2	; negate R2 so that we can easily compare the value to another register with ADD
		ADD R2, R2, #1	; R2 should now be 0x0001
		LD R0, ARRAY	; R0 <- M[ARRAY] = 0x5000
		LDR R3, R0, #0	; test for empty array
		ADD R3, R2, R3	; R3 = R3 + 1
		BRz DONE	; R3 must have been #-1, which means we have an empty array
		
		ADD R1, R0, #0	; R1 = R0
TRAVERSE	ADD R1, R1, #1	; increment R1 until it is at the end of the array
		LDR R3, R1, #0	; test for end of array
		ADD R3, R2, R3	; R3 = R3 + 1
		BRnp TRAVERSE	; if not #-1, then keep traversing

		ADD R1, R1, #-1	; we want R1 to point at end of array, not the sentinel

		; At this point, R0 should point to the first element and R1 should point to the last element

LOOP		JSR SWAP	; swap the array contents of R0 and R1
		ADD R0, R0, #1	; increment R0
		ADD R1, R1, #-1	; decrement R1
		
		; Now check if R0 == R1 or R0 == R1 - 1.
		; If there are an odd number of elements, then if R0 == R1 then we have swapped every element and we are done
		; If there are an even number of elements, then if R0 == R1 - 1 (they are one apart) then we have swapped every element and we are done

		NOT R2, R1	; negate R1
		ADD R2, R2, #1
		ADD R2, R0, R2	; R2 = R0 - R1
		BRz DONE	; if R0 - R1 == 0, we are done
		ADD R2, R2, #1	; R2 = R0 - R1 + 1
		BRz DONE	; if R0 - R1 == -1, we are done
		BR LOOP		; repeat until done

		; This is our subroutine. It swaps the memory locations of R0 and R1 (M[R0] <-> M[R1])

SWAP		ST R2, SAVER2	; save the registers that we will overwrite
		ST R3, SAVER3
		
		LDR R2, R0, #0	; R2 = M[R0]
		LDR R3, R1, #0	; R3 = M[R1]
		STR R2, R1, #0	; M[R1] = R2
		STR R3, R0, #0	; M[R0] = R3
		
		LD R2, SAVER2	; restore the original register values
		LD R3, SAVER3
		RET
		
DONE		TRAP x25

ARRAY		.FILL x5000	; location of array
SENTINEL	.FILL xFFFF	; designates the end of our array
SAVER2		.BLKW 1
SAVER3		.BLKW 1
		.END
