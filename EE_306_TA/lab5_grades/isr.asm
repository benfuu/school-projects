.ORIG x4000
		
;	Keyboard Interrupt Service Routine
;	Is called when the user types a key 
;
;

KBISR	ST	R3, SAVER3
	ST	R2, SAVER2
	LDI	R0, KBDR		;Reading KBDR clears bit 15 of KBSR
	
	LD R5 TEN;
AHH	STI R0  DDR;
	ADD R5 R5 #-1;
	BRNP AHH;


LEAVE	LD	R3, SAVER3
	LD	R2, SAVER2
	RTI



	


SAVER3	.BLKW	1
SAVER2	.BLKW	1
SAVER7	.BLKW	1
KBDR	.FILL	xFE02
DDR	.FILL	xFE06
TEN	.FILL	#10;
.END