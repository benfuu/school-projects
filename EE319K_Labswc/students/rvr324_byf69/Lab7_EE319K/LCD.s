; LCD.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly

; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for the Kentec EB-LM4F120-L35
; This is the lowest level driver that interacts directly with hardware
; As part of Lab 7, students need to implement these three functions
;
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

      EXPORT   LCD_GPIOInit
      EXPORT   LCD_WriteCommand
      EXPORT   LCD_WriteData

      AREA    |.text|, CODE, READONLY, ALIGN=2
      THUMB
      ALIGN          
SYSCTL_RCGC2_R          EQU   0x400FE108
GPIO_PORTA_DATA_R       EQU   0x400043FC
GPIO_PORTA_DIR_R        EQU   0x40004400
GPIO_PORTA_AFSEL_R      EQU   0x40004420
GPIO_PORTA_DEN_R        EQU   0x4000451C
GPIO_PORTB_DATA_R       EQU   0x400053FC
GPIO_PORTB_DIR_R        EQU   0x40005400
GPIO_PORTB_AFSEL_R      EQU   0x40005420
GPIO_PORTB_DEN_R        EQU   0x4000551C

; ************** LCD_GPIOInit ****************************
; Initializes Ports A and B for Kentec EB-LM4F120-L35
; Port A bits 4-7 are output to four control signals
; Port B bits 0-7 are output data is the data bus 
; Initialize all control signals high (off)
;  PA4     RD  Read control signal             --------PA6 | PA5 | PA4 |
;  PA6     RS  Register/Data select signal     | CS  | RS  | WR  | RD  |
;  PA7     CS  Chip select signal    -----------------
;  PA5     WR  Write control signal            | PA7 |           -------------------------
; wait 40 us
; Invariables: This function must not permanently modify registers R4 to R11
LCD_GPIOInit
	LDR R1, =SYSCTL_RCGC2_R
	LDR R0, [R1]
	ORR R0, #0x03		;initialize Port A and B clock
	STR R0, [R1]
	NOP
	NOP
	LDR R1, =GPIO_PORTA_DIR_R
	LDR R0, [R1]
	ORR R0, #0xF0			;set portA bits[7-4] to 1
	STR R0, [R1]
	LDR R1, =GPIO_PORTB_DIR_R
	LDR R0, [R1]
	ORR R0, #0xFF			;set portB bits[7-0] to 1
	STR R0, [R1]
	LDR R1, =GPIO_PORTA_AFSEL_R
	LDR R0, [R1]
	AND R0, #0x0F			;turn off portA AFSEL
	STR R0, [R1]
	LDR R1, =GPIO_PORTB_AFSEL_R
	LDR R0, [R1]
	AND R0, #0x00			;turn off portB AFSEL
	STR R0, [R1]
	LDR R1, =GPIO_PORTA_DEN_R
	LDR R0, [R1]
	ORR R0, #0xF0			;enable
	STR R0, [R1]
	LDR R1, =GPIO_PORTB_DEN_R
	LDR R0, [R1]
	ORR R0, #0xFF			;enable
	STR R0, [R1]
    LDR R1, =GPIO_PORTA_DATA_R
	LDR R0, [R1]
	ORR R0, #0xF0			;make control signals high (OFF)
	STR R0, [R1]
	;delay sequence 
	MOV R1,#640				;set R1 to a 1us delay
delay
	SUB R1,#1 
	CMP R1,#0 
	BNE delay  	 
	BX  LR

;* * * * * * * * End of LCD_GPIOInit * * * * * * * * 


; ************** LCD_WriteCommand ************************
; - Writes an 8-bit command to the LCD controller
; - RS low during command write
; 8-bit command passed in R0
; 1) LCD_DATA = 0x00;    // Write 0 as MSB of command 
; 2) LCD_CTRL = 0x10;    // Set CS, WR, RS low
; 3) LCD_CTRL = 0x70;    // Set WR and RS high
; 4) LCD_DATA = command; // Write 8-bit LSB command 
; 5) LCD_CTRL = 0x10;    // Set WR and RS low
; 6) wait 2 bus cycles     
; 7) LCD_CTRL = 0xF0;    // Set CS, WR, RS high
; ********************************************************
; Invariables: This function must not permanently modify registers R4 to R11
LCD_WriteCommand
	AND R3, #0
	LDR R2, =GPIO_PORTB_DATA_R	;R2=address PortB,data
	STRB R3, [R2]				;write 0 as MSB of command
	LDR R1, =GPIO_PORTA_DATA_R	;R1=address PortA,ctrl
	LDR R3, [R1]
	AND R3, #0x10				;set CS, RS, and WR low (RD always high for command)
	STRB R3, [R1]
	ORR R3, #0x70				;set RS and WR High
	STRB R3, [R1]
	STRB R0, [R2]				;write command (in R0) to LCD_DATA
	LDR R2, [R1]
	AND R2, #0x10				;set RS and WR low
	STRB R2, [R1]
	NOP
	NOP							;waiting two bus cycles
	LDR R3, [R1]
	ORR R3, #0xF0				;set CS, RS, WR high
	STRB R3, [R1]
	BX  LR
	
;* * * * * * * * End of LCD_WriteCommand * * * * * * * * 

; ************** LCD_WriteData ***************************
; - Writes 16-bit data to the LCD controller
; - RS high during data write
; 16-bit data passed in R0
; 1) LCD_DATA = (data>>8);  // Write MSB to LCD data bus
; 2) LCD_CTRL = 0x50;       // Set CS, WR low
; 3) LCD_CTRL = 0x70;       // Set WR high
; 4) LCD_DATA = data;       // Write LSB to LCD data bus 
; 5) LCD_CTRL = 0x50;       // Set WR low
; 6) wait 2 bus cycles     
; 7) LCD_CTRL = 0xF0;       // Set CS, WR high
; ********************************************************
; Invariables: This function must not permanently modify registers R4 to R11
LCD_WriteData
	MOV R1, R0
	AND R1, #0xFF00			;mask for MSB
	LSR R1, #8				;R1=shifted MSB
	AND R0, #0x00FF			;R0=LSB
	LDR	R3, =GPIO_PORTB_DATA_R		;R3=address PORTB data
	STRH R1, [R3]			;write MSB to LCD data bus
	LDR R2, =GPIO_PORTA_DATA_R		;R2=address PORTA data
	LDR R1, [R2]
    AND R1, #0x50			;set CS, WR low
	STRB R1, [R2]
 	ORR R1, #0x70			;set WR high
	STRH R1, [R2]			;store in PORTA data 
	STRH R0, [R3]			;write LSB to LCD data bus
	LDR R1, [R2]
	AND R1, #0x50
	STRH R1, [R2]			;set WR low
	NOP
	NOP
	LDR R1, [R2]
	ORR R1, #0xF0
	STRH R1, [R2]
	BX  LR

;* * * * * * * * End of LCD_WriteData * * * * * * * * 




    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
    