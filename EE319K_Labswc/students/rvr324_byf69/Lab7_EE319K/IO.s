; IO.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly

; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for the switch and LED
; You are allowed to use any switch and any LED, 
; although the Lab suggests the SW1 switch PF4 and Red LED PF1

; As part of Lab 7, students need to implement these three functions

;  This example accompanies the book
;  "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
;  ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013
;
;Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
;   You may use, edit, run or distribute this file
;   as long as the above copyright notice remains
;THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
;OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
;MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
;VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
;OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
;For more information about my classes, my research, and my books, see
;http://users.ece.utexas.edu/~valvano/

; negative logic SW2 connected to PF0 on the Launchpad
; red LED connected to PF1 on the Launchpad
; blue LED connected to PF2 on the Launchpad
; green LED connected to PF3 on the Launchpad
; negative logic SW1 connected to PF4 on the Launchpad

        EXPORT   IO_Init
        EXPORT   IO_Touch
        EXPORT   IO_HeartBeat

GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_AFSEL_R EQU 0x40025420
GPIO_PORTF_PUR_R   EQU 0x40025510
GPIO_PORTF_DEN_R   EQU 0x4002551C
GPIO_PORTF_LOCK_R  EQU 0x40025520
GPIO_PORTF_CR_R    EQU 0x40025524
GPIO_PORTF_AMSEL_R EQU 0x40025528
GPIO_PORTF_PCTL_R  EQU 0x4002552C
GPIO_LOCK_KEY      EQU 0x4C4F434B  ; Unlocks the GPIO_CR register

SYSCTL_RCGC2_R     EQU 0x400FE108

        AREA    |.text|, CODE, READONLY, ALIGN=2
        THUMB

;------------IO_Init------------
; Initialize GPIO Port for a switch and an LED
; Input: none
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
IO_Init
    LDR	R0,=SYSCTL_RCGC2_R			;enable PORTF
	LDR	R1,[R0]
	ORR	R1,#0x20
	STR	R1,[R0]
	NOP
	NOP
	
	LDR	R0,=GPIO_PORTF_DIR_R		;set PF0 and PF4 as inputs, PF3-1 as outputs
	LDR	R1,[R0]
	ORR	R1,#0x0E
	STR	R1,[R0]
	
	LDR	R0,=GPIO_PORTF_AFSEL_R		;clear PF4-0
	LDR	R1,[R0]
	AND	R1,#0xE1
	STR	R1,[R0]
	
	LDR	R0,=GPIO_PORTF_DEN_R		;enable PF4-0
	LDR	R1,[R0]
	ORR	R1,#0x1E
	STR	R1,[R0]
	
	LDR	R0,=GPIO_PORTF_PUR_R			;enable PUR for PF0 and PF4
	LDR	R1,[R0]
	ORR	R1,#0x10
	STR	R1,[R0]
	
    BX  LR      
;* * * * * * * * End of IO_Init * * * * * * * * 

;------------IO_HeartBeat------------
; Toggle the output state of the  LED.
; Input: none
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
IO_HeartBeat
    LDR	R0,=GPIO_PORTF_DATA_R		;toggle PF3-1
	LDR	R1,[R0]
	EOR	R1,#0x0E
	STR	R1,[R0]
    BX  LR              

;* * * * * * * * End of IO_HeartBeat * * * * * * * * 

;------------IO_Touch------------
; wait for release and touch of the switch
; Input: none
; Output: none
; This is a public function
; Invariables: This function must not permanently modify registers R4 to R11
IO_Touch  
	LDR	R0,=GPIO_PORTF_DATA_R
	LDR	R1,[R0]
	AND	R1,#0x10			;see if switch is originally pressed
	CMP	R1,#0x00
	BEQ	Pressed
	CMP	R1,#0x0E
	BEQ	Pressed
NotPressed					;wait for switch to be pressed
	LDR	R1,[R0]
	CMP	R1,#0x00
	BEQ	Done
	CMP	R1,#0x0E
	BEQ	Done
	B	NotPressed
Pressed
	LDR	R1,[R0]
	CMP	R1,#0x10			;wait for switch to be not pressed
	BEQ	Done
	CMP	R1,#0x1E
	BEQ	Done
	B	Pressed
Done
    BX  LR      
;* * * * * * * * End of IO_Touch * * * * * * * * 

    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file