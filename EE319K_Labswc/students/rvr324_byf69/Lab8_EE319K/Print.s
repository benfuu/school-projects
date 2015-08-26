; print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for the Kentec EB-LM4F120-L35
;
; As part of Lab 7, students need to implement these two functions

;  Data pin assignments:
;  PB0-7   LCD parallel data input
;
;  Control pin assignments:
;  PA4     RD  Read control signal             -------------------------
;  PA5     WR  Write control signal            | PA7 | PA6 | PA5 | PA4 |
;  PA6     RS  Register/Data select signal     | CS  | RS  | WR  | RD  |
;  PA7     CS  Chip select signal              -------------------------
;
;  Touchpad pin assignments:
;  PA2     Y-                                  -------------  -------------
;  PA3     X-                                  | PA3 | PA2 |  | PE5 | PE4 |
;  PE4     X+    AIN9                          | X-  | Y-  |  | Y+  | X+  |
;  PE5     Y+    AIN8                          -------------  -------------

    IMPORT   LCD_OutChar
    IMPORT   LCD_Goto
    IMPORT   LCD_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix
		
	AREA	DATA, ALIGN=2
Link1	SPACE	8
Link2	SPACE	8
Counter	SPACE	8
    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB
    ALIGN          

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number 
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	LDR	R1,=Link1		;save LR
	STR	LR,[R1]
	LDR	R1,=Counter		;initialize counter to be used in program
	MOV	R2,#0
	STR	R2,[R1]
	
n	EQU	0				;bind n as a variable on stack

	MOV	R2,#0
	MOV	R1,#12			;R1 is counter
InitializeStack_Dec
	STR	R2,[SP]			;clear all locations (set to null)
	SUB	SP,#8			;allocate 10 32-bit numbers (null terminated on both sides)
	SUB	R1,#1
	CMP	R1,#0
	BNE	InitializeStack_Dec
	
	ADD	SP,#16			;make sure SP points to top of stack (right address = 0x200003C8)
	MOV	R3,#10000		;R3 = 10^4
	MOV	R12,#10
	MUL	R3,R3
	MUL	R3,R12			;R3 = 10^9
	
Divide_Dec
	CMP	R0,R3			;see if R3 is too big to be divided	
	BLO	NotEnough_Dec
	B	Print_Dec
NotEnough_Dec
	UDIV R3,R12
	CMP	R3,#0
	BEQ	PrintNull_Dec
	B	Divide_Dec
Print_Dec
	CMP	R0,R3			;see if character is a 0
	BLO	PrintZero_Dec
	UDIV R1,R0,R3		;get first char into R0
	MUL	R2,R1,R3		;R2 = (R1/R3)*R3 without the remainder
	SUB	R0,R2			;R0 = remainder (new number)
	ADD	R1,#0x30		;convert to ASCII
	STR	R1,[SP,#n]		;push ASCII character to stack
	ADD	SP,#8			;increment the SP
	B	Next_Dec
	
PrintZero_Dec
	MOV	R1,#0x30		;print "0"
	STR	R1,[SP,#n]		;push 0x30 (ASCII for 0) to stack
	ADD	SP,#8
	B	Next_Dec
	
Next_Dec
	CMP	R3,#1			;see if last char has been outputted (checking the ones place)
	BEQ	Done_Dec
	UDIV R3,R12			;R3 = R3/10 (check next place)
	B	Print_Dec
	
PrintNull_Dec
	MOV	R0,#0x30
	BL	LCD_OutChar
	B	Finished_Dec
	
Done_Dec
RecalcSP	
	SUB	SP,#8			;subtract by how many times SP was incremented
	LDR	R1,[SP]			;check if string has reached null terminated ending
	CMP	R1,#0
	BNE	RecalcSP
	
	ADD	SP,#8			;SP points to correct address
OutChar_Dec	
	LDR	R0,[SP,#n]		;output character onto LCD
	BL	LCD_OutChar
	ADD	SP,#8			;increment SP
	LDR	R1,[SP]			;check for null termination
	CMP	R1,#0
	BNE	OutChar_Dec

	B	Finished_Dec
	
Finished_Dec
	ADD	SP,#8			;deallocate so SP points to bottom of stack
	LDR	R1,[SP]
	CMP	R1,#0
	BEQ	Finished_Dec	;continue until SP points to one past 0x20000418
	
	SUB	SP,#8			;reset SP to 0x20000418
	LDR	R1,=Link1		;restore LR
	LDR	LR,[R1]
	
	BX	LR
;* * * * * * * * End of LCD_OutDec * * * * * * * * 

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999 
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 " 
;       R0=3,    then output "0.003 " 
;       R0=89,   then output "0.089 " 
;       R0=123,  then output "0.123 " 
;       R0=9999, then output "9.999 " 
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
Digit1	EQU	0
Digit2	EQU	8
Digit3	EQU	16
Digit4	EQU	24

	LDR	R1,=Link2		;save LR
	STR	LR,[R1]
	
	SUB	SP,#24			;allocate space for variables
	MOV	R1,#9999		;see if the character in R0 is greater than 9999
	CMP	R0,R1
	BHI	TooBig
	
	MOV	R3,#1000		;R3 = 10^3
	MOV	R12,#10
	MOV	R1,R0			;transfer number into R1

Print_Outfix
	CMP	R1,R3			;see if character is a 0
	BLO	PrintZero_Outfix
	UDIV R0,R1,R3		;get first char into R0
	MUL	R2,R0,R3		;R2 = (R1/R3)*R3 without the remainder
	SUB	R1,R2			;R1 = remainder (new number)
	ADD	R0,#0x30		;convert to ASCII
	STR	R0,[SP]			;push ASCII character to stack
	ADD	SP,#8
	B	Next_Outfix
PrintZero_Outfix
	MOV	R0,#0x30		;print a 0
	STR	R0,[SP]			;push 0x30 (ASCII for 0) to stack
	ADD	SP,#8
	B	Next_Outfix
Next_Outfix
	CMP	R3,#1			;see if last char has been outputted
	BEQ	Done_Outfix
	UDIV R3,R12			;R3 = R3/10
	B	Print_Outfix
TooBig
	MOV	R0,#0x2A		;"*"
	BL	LCD_OutChar
	MOV	R0,#0x2E		;"."
	BL	LCD_OutChar
	MOV	R0,#0x2A		;"*"
	BL	LCD_OutChar
	MOV	R0,#0x2A		;"*"
	BL	LCD_OutChar
	MOV	R0,#0x2A		;"*"
	BL	LCD_OutChar
	B	Finished_Outfix
Done_Outfix
	SUB	SP,#32
	LDR	R0,[SP,#Digit1]
	BL	LCD_OutChar
	MOV	R0,#0x2E
	BL	LCD_OutChar
	LDR	R0,[SP,#Digit2]
	BL	LCD_OutChar
	LDR	R0,[SP,#Digit3]
	BL	LCD_OutChar
	LDR	R0,[SP,#Digit4]
	BL	LCD_OutChar
	B	Finished_Outfix
Finished_Outfix
	ADD	SP,#24			;deallocate stack
	LDR	R1,=Link2		;restore LR
	LDR	LR,[R1]
	
    BX	LR
;* * * * * * * * End of LCD_OutFix * * * * * * * * 

   
    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
    