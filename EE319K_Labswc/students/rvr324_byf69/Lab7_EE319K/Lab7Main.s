; Lab7Main.s
; Runs on LM4F120 or TM4C123
; Lab 7 test main program
; I/O drivers for Kentec EB-LM4F120-L35
; October 29, 2013

;  This example accompanies the book
;  "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
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

       AREA    |.text|, CODE, READONLY, ALIGN=2
       THUMB
       EXPORT   Start
       IMPORT   PLL_Init
       IMPORT   LCD_Init
       IMPORT   LCD_OutChar
       IMPORT   LCD_Goto
       IMPORT   LCD_ColorFill
       IMPORT   LCD_SetTextColorRGB
       IMPORT   LCD_OutString
       IMPORT   LCD_OutDec   ; you write this
       IMPORT   LCD_OutFix   ; you write this      
       IMPORT   IO_Init      ; you write this
       IMPORT   IO_Touch     ; you write this
       IMPORT   IO_HeartBeat ; you write this
; color definitions encoded as 5-6-5 RGB
BLACK       EQU  0x0000  ; R=0   G=0   B=0
WHITE       EQU  0xFFFF  ; R=255 G=255 B=255
PINK        EQU  0xF81F  ; R=255 G=0   B=255
RED         EQU  0xF800  ; R=255 G=0   B=0
BURNTORANGE EQU  0xC300  ; R=197 G=96  B=6
ORANGE      EQU  0xFC00  ; R=255 G=128 B=0
YELLOW      EQU  0xFFE0  ; R=255 G=255 B=0
GREEN       EQU  0x07E0  ; R=0   G=255 B=0
LIGHTBLUE   EQU  0x041F  ; R=0   G=128 B=255
BLUE        EQU  0x001F  ; R=0   G=0   B=255
VIOLET      EQU  0x801F  ; R=128 G=0   B=255

       
Start  BL   PLL_Init      ; initialize PLL 80 MHz
       BL   LCD_Init      ; initialize Kentec EB-LM4F120-L35
       BL   IO_Init      ; ***Your function that initialize switch and LED
       MOV  R0,#YELLOW
       BL   LCD_SetTextColorRGB
run    MOV  R0,#BLACK
       BL   LCD_ColorFill
       MOV  R0,#0        ;left column
       MOV  R1,#0        ;top row 
       BL   LCD_Goto
       LDR  R0,=Welcome
       BL   LCD_OutString ;This is provided for you
       LDR  R4,=TestData
       LDR  R5,=TestEnd
       MOV  R6,#2        ;row number
       BL   IO_Touch     ;***Your function that waits for release and touch 
loop   BL   IO_HeartBeat ;***Your function that toggles LED
       MOV  R0,#0        ;left column
       MOV  R1,R6        ;row number
       BL   LCD_Goto
       LDR  R0, [R4]            
       BL   LCD_OutDec   ;***Your function that outputs an integer
       MOV  R0,#12       ;13th column
       MOV  R1,R6        ;row number
       BL   LCD_Goto
       LDR  R0, [R4],#4           
       BL   LCD_OutFix   ;***Your function that outputs a fixed-point
       BL   IO_Touch     ;***Your function that waits for release and touch 
       ADD  R6,#1        ;next row
       CMP  R4, R5
       BNE  loop      
       B    run    
         ALIGN          
Welcome  DCB "Lab 7, welcome to 319K!",0
         ALIGN          
TestData DCD 0,7,34,199,321,654,4789,9999,10000,21896,65535
         DCD 123456,1234567,12345678,123456789,0xFFFFFFFF
TestEnd  DCD 0

      ALIGN         ; make sure the end of this section is aligned
      END           ; end of file