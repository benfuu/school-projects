// SSD2119.c
// Steven Prickett (steven.prickett@gmail.com)
// August 23, 2013
// Runs on LM4F120 or TM4C123

// Driver for the SSD2119 interface on a Kentec 320x240x16 BoosterPack
// - Uses all 8 bits on PortB for writing data to LCD 
//   and bits 4-7 on Port A for control signals
// - Adapted from original Kentec320x240x16_ssd2119_8bit.c driver by TI

//Copyright 2013 by Steven Prickett (steven.prickett@gmail.com)
//   You may use, edit, run or distribute this file
//   as long as the above copyright notice remains
//THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
//OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
//MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
//PRICKETT SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
//OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

// TODO:
// - bitmap drawing
// - touch support
// 
//  Data pin assignments:
//  PB0-7   LCD parallel data input
//
//  Control pin assignments:
//  PA4     RD  Read control signal             -------------------------
//  PA5     WR  Write control signal            | PA7 | PA6 | PA5 | PA4 |
//  PA6     RS  Register/Data select signal     | CS  | RS  | WR  | RD  |
//  PA7     CS  Chip select signal              -------------------------
//
//  Touchpad pin assignments:
//  PA2     Y-                                  -------------  -------------
//  PA3     X-                                  | PA3 | PA2 |  | PE5 | PE4 |
//  PE4     X+    AIN9                          | X-  | Y-  |  | Y+  | X+  |
//  PE5     Y+    AIN8                          -------------  -------------
//   
//  Touchscreen resistance measurements:
//  -------------------             
//  |1               2|                XN->YP       XP->YN    
//  |                 |         1       1150         1400
//  |                 |         2       640           800
//  |        5        |         3       1400         1100
//  |                 |         4       870           580
//  |3               4|         5       1000          960                   
//  ------------------- 
//
//  XP->XN = 651

#include "LCD.h"

unsigned short cursorX;
unsigned short cursorY;
unsigned short textColor;

// This table contains the hex values that represent pixels
// for a font that is 5 pixels wide and 8 pixels high
static const char ASCII[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}  // 20
    ,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
    ,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
    ,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
    ,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
    ,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
    ,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
    ,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
    ,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
    ,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
    ,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
    ,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
    ,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
    ,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
    ,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
    ,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
    ,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
    ,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
    ,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
    ,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
    ,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
    ,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
    ,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
    ,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
    ,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
    ,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
    ,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
    ,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
    ,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
    ,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
    ,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
    ,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
    ,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
    ,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
    ,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
    ,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
    ,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
    ,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
    ,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
    ,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
    ,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
    ,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
    ,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
    ,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
    ,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
    ,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
    ,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
    ,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
    ,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
    ,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
    ,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
    ,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
    ,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
    ,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
    ,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
    ,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
    ,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
    ,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
    ,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
    ,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
    ,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c '\'
    ,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
    ,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
    ,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
    ,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
    ,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
    ,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
    ,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
    ,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
    ,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
    ,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
    ,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
    ,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
    ,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
    ,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
    ,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
    ,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
    ,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
    ,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
    ,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
    ,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
    ,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
    ,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
    ,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
    ,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
    ,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
    ,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
    ,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
    ,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
    ,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
    ,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
    ,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
    ,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
    ,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
    ,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ~
 //   ,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f DEL
    ,{0x1f, 0x24, 0x7c, 0x24, 0x1f} // 7f UT sign
};

// converts 24bit RGB color to display color
//#define CONVERT24BPP(c) ( (((c) & 0x00f80000) >> 8) | (((c) & 0x0000fc00) >> 5) | (((c) & 0x000000f8) >> 3) )
#define CONVERT24BPP(c) ( (((c) & 0x00f80000) >> 8) | (((c) & 0x0000fc00) >> 5) | (((c) & 0x000000f8) >> 3) )

// converts 8bit greyscale to display color
#define CONVERT8BPP(c)  ( (((c) >> 3) << 11) | (((c) >> 2) << 5 ) | ((c) >> 3) )

// converts 4bit greyscale to display color
#define CONVERT4BPP(c)  ( ((c) << 12) | ((c) << 7 ) | ((c) << 1) )

// define BMP offsets
#define BMP_WIDTH_OFFSET        0x0012
#define BMP_HEIGHT_OFFSET       0x0016
#define BMP_DATA_OFFSET         0x000A
#define BMP_BPP_OFFSET          0x001C

// define command codes
#define SSD2119_DEVICE_CODE_READ_REG    0x00
#define SSD2119_OSC_START_REG           0x00
#define SSD2119_OUTPUT_CTRL_REG         0x01
#define SSD2119_LCD_DRIVE_AC_CTRL_REG   0x02
#define SSD2119_PWR_CTRL_1_REG          0x03
#define SSD2119_DISPLAY_CTRL_REG        0x07
#define SSD2119_FRAME_CYCLE_CTRL_REG    0x0B
#define SSD2119_PWR_CTRL_2_REG          0x0C
#define SSD2119_PWR_CTRL_3_REG          0x0D
#define SSD2119_PWR_CTRL_4_REG          0x0E
#define SSD2119_GATE_SCAN_START_REG     0x0F
#define SSD2119_SLEEP_MODE_REG          0x10
#define SSD2119_ENTRY_MODE_REG          0x11
#define SSD2119_GEN_IF_CTRL_REG         0x15
#define SSD2119_PWR_CTRL_5_REG          0x1E
#define SSD2119_RAM_DATA_REG            0x22
#define SSD2119_FRAME_FREQ_REG          0x25
#define SSD2119_VCOM_OTP_1_REG          0x28
#define SSD2119_VCOM_OTP_2_REG          0x29
#define SSD2119_GAMMA_CTRL_1_REG        0x30
#define SSD2119_GAMMA_CTRL_2_REG        0x31
#define SSD2119_GAMMA_CTRL_3_REG        0x32
#define SSD2119_GAMMA_CTRL_4_REG        0x33
#define SSD2119_GAMMA_CTRL_5_REG        0x34
#define SSD2119_GAMMA_CTRL_6_REG        0x35
#define SSD2119_GAMMA_CTRL_7_REG        0x36
#define SSD2119_GAMMA_CTRL_8_REG        0x37
#define SSD2119_GAMMA_CTRL_9_REG        0x3A
#define SSD2119_GAMMA_CTRL_10_REG       0x3B
#define SSD2119_V_RAM_POS_REG           0x44
#define SSD2119_H_RAM_START_REG         0x45
#define SSD2119_H_RAM_END_REG           0x46
#define SSD2119_X_RAM_ADDR_REG          0x4E
#define SSD2119_Y_RAM_ADDR_REG          0x4F
#define ENTRY_MODE_DEFAULT              0x6830

// dimensions of the LCD in pixels
#define LCD_HEIGHT      240
#define LCD_WIDTH       320

// number of 5x8 characters that will fit on the screen
#define MAX_CHARS_X     53
#define MAX_CHARS_Y     26

// entry mode macros
#define HORIZ_DIRECTION     0x28
#define VERT_DIRECTION      0x20
#define ENTRY_MODE_DEFAULT  0x6830   // 0110.1000.0011.0000
#define MAKE_ENTRY_MODE(x) ((ENTRY_MODE_DEFAULT & 0xFF00) | (x))

// bit-banded addresses for port stuff
#define LCD_RD_PIN       (*((volatile unsigned long *)0x40004040))     // PA4
#define LCD_WR_PIN       (*((volatile unsigned long *)0x40004080))     // PA5
#define LCD_RS_PIN       (*((volatile unsigned long *)0x40004100))     // PA6
#define LCD_CS_PIN       (*((volatile unsigned long *)0x40004200))     // PA7
#define LCD_CTRL         (*((volatile unsigned long *)0x400043C0))     // PA4-7
#define LCD_DATA         (*((volatile unsigned long *)0x400053FC))     // PB0-7



// ************** LCD_Init ********************************
// - Initializes the LCD
// - Command sequence verbatim from original driver
// ********************************************************
void LCD_Init(void){
    unsigned long count = 0;

    LCD_GPIOInit();

    // Enter sleep mode (if we are not already there).
    LCD_WriteCommand(SSD2119_SLEEP_MODE_REG);
    LCD_WriteData(0x0001);

    // Set initial power parameters.
    LCD_WriteCommand(SSD2119_PWR_CTRL_5_REG);
    LCD_WriteData(0x00BA);
    LCD_WriteCommand(SSD2119_VCOM_OTP_1_REG);
    LCD_WriteData(0x0006);

    // Start the oscillator.
    LCD_WriteCommand(SSD2119_OSC_START_REG);
    LCD_WriteData(0x0001);

    // Set pixel format and basic display orientation (scanning direction).
    LCD_WriteCommand(SSD2119_OUTPUT_CTRL_REG);
    LCD_WriteData(0x72EF);                                  //0x72EF = 0,0 in top left, scan right  
    LCD_WriteCommand(SSD2119_LCD_DRIVE_AC_CTRL_REG);        //0x30EF = 0,0 in bottom right, scan left
    LCD_WriteData(0x0600);                                  //0x32EF = 0,0 in top right, scan left

    // Exit sleep mode.
    LCD_WriteCommand(SSD2119_SLEEP_MODE_REG);
    LCD_WriteData(0x0000);

    // Delay 30mS
    for (count = 0; count < 200000; count++) {}

    // Configure pixel color format and MCU interface parameters.
    LCD_WriteCommand(SSD2119_ENTRY_MODE_REG);
    LCD_WriteData(ENTRY_MODE_DEFAULT);

    // Enable the display.
    LCD_WriteCommand(SSD2119_DISPLAY_CTRL_REG);
    LCD_WriteData(0x0033);

    // Set VCIX2 voltage to 6.1V.
    LCD_WriteCommand(SSD2119_PWR_CTRL_2_REG);
    LCD_WriteData(0x0005);

    // Configure gamma correction.
    LCD_WriteCommand(SSD2119_GAMMA_CTRL_1_REG);
    LCD_WriteData(0x0000);
    LCD_WriteCommand(SSD2119_GAMMA_CTRL_2_REG);
    LCD_WriteData(0x0400);
    LCD_WriteCommand(SSD2119_GAMMA_CTRL_3_REG);
    LCD_WriteData(0x0106);
    LCD_WriteCommand(SSD2119_GAMMA_CTRL_4_REG);
    LCD_WriteData(0x0700);
    LCD_WriteCommand(SSD2119_GAMMA_CTRL_5_REG);
    LCD_WriteData(0x0002);
    LCD_WriteCommand(SSD2119_GAMMA_CTRL_6_REG);
    LCD_WriteData(0x0702);
    LCD_WriteCommand(SSD2119_GAMMA_CTRL_7_REG);
    LCD_WriteData(0x0707);
    LCD_WriteCommand(SSD2119_GAMMA_CTRL_8_REG);
    LCD_WriteData(0x0203);
    LCD_WriteCommand(SSD2119_GAMMA_CTRL_9_REG);
    LCD_WriteData(0x1400);
    LCD_WriteCommand(SSD2119_GAMMA_CTRL_10_REG);
    LCD_WriteData(0x0F03);

    // Configure Vlcd63 and VCOMl.
    LCD_WriteCommand(SSD2119_PWR_CTRL_3_REG);
    LCD_WriteData(0x0007);
    LCD_WriteCommand(SSD2119_PWR_CTRL_4_REG);
    LCD_WriteData(0x3100);

    // Set the display size and ensure that the GRAM window is set to allow
    // access to the full display buffer.
    LCD_WriteCommand(SSD2119_V_RAM_POS_REG);
    LCD_WriteData((LCD_HEIGHT-1) << 8);
    LCD_WriteCommand(SSD2119_H_RAM_START_REG);
    LCD_WriteData(0x0000);
    LCD_WriteCommand(SSD2119_H_RAM_END_REG);
    LCD_WriteData(LCD_WIDTH-1);
    LCD_WriteCommand(SSD2119_X_RAM_ADDR_REG);
    LCD_WriteData(0x00);
    LCD_WriteCommand(SSD2119_Y_RAM_ADDR_REG);
    LCD_WriteData(0x00);

    // Clear the contents of the display buffer.
    LCD_WriteCommand(SSD2119_RAM_DATA_REG);
    for(count = 0; count < (320 * 240); count++){
        LCD_WriteData(0x0000);
    }
    // Set text cursor to top left of screen
    LCD_SetCursor(0, 0);
    
    // Set default text color to white
    LCD_SetTextColor(255, 255, 255);
}

// ************** convertColor ****************************
// - Converts 8-8-8 RGB values into 5-6-5 RGB
// ********************************************************
unsigned short convertColor(unsigned char r, unsigned char g, unsigned char b){
    return ((r>>3)<<11) | ((g>>2)<<5) | (b>>3);
}

// ************** LCD_ColorFill ***************************
// - Fills the screen with the specified color
// ********************************************************
void LCD_ColorFill(unsigned short color){
    LCD_DrawFilledRect(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}

// ************** abs *************************************
// - Returns the absolute value of an integer
// - Used to help with circle drawing
// ********************************************************
int abs(int a){
    if (a < 0) return -a;
    else return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                 PRINTING FUNCTIONS                                            //
///////////////////////////////////////////////////////////////////////////////////////////////////

// ************** LCD_OutChar ***************************
// - Prints a character to the screen
// R0 has 8-bit ASCII character
// ********************************************************
void LCD_OutChar(unsigned char data){
    unsigned char i,j,tempData;

    // Return cursor to new line if requested
    if (data == '\n') {
        LCD_SetCursor(0, cursorY + 9);
    }    

    // Don't print characters outside of this range
    if (data < 0x20 || data > 0x7f) return;  

    // If character would print beyond LCD_WIDTH, go to new line
    if (cursorX + 5 >= LCD_WIDTH) {
        LCD_SetCursor(0, cursorY + 9);
    }

    // If character would print beyond LCD_HEIGHT, return to top of screen
    if (cursorY + 8 >= LCD_HEIGHT) {
        LCD_SetCursor(cursorX, 0);
    }

    // Print our character
    for(i=0; i<5; i=i+1){
        tempData = ASCII[data - 0x20][i];
        for (j=0; j<8; j=j+1){
            // This would print transparent letters
            // if (tempData & 0x01) {
            //     LCD_DrawPixel(cursorX + i, cursorY + j, textColor);
            // }
            
            // This will overwrite the entire character block (non-transparent)
            LCD_DrawPixel(cursorX + i, cursorY + j, (tempData & 0x01) * textColor);
            
            // Shift to our next pixel
            tempData = tempData >> 1;
        }
    }
    
    // Set cursor to next location
    LCD_SetCursor(cursorX + 6, cursorY);
}

// ************** LCD_OutString *************************
// - Prints a string to the screen
// R0 is pointer to null-terminated ASCII string
// ********************************************************
void LCD_OutString(char data[]){
  unsigned short i = 0;
    
  // While data[i] is not a null terminator, print out characters
  while (data[i] != 0){
    LCD_OutChar(data[i]);
    i += 1;
  }
}

// ************** LCD_SetCursor ***************************
// - Sets character printing cursor position
// xPos is R0 column pixel number 0 (left) to 319 (right)
// yPos is R1 row pixel number 0 (top) to 239 (bottom)
// ********************************************************
void LCD_SetCursor(unsigned short xPos, unsigned short yPos){
  cursorX = xPos; // Set the X address of the display cursor
  cursorY = yPos; // Set the Y address of the display cursor
}

// ************** LCD_Goto ********************************
// - Sets character printing cursor position in terms of 
//   character positions rather than pixels. 
// - Ignores invalid position requests.
// x is R0 column number 0 (left) to 52 (right)
// y is R1 row number 0 (top) to 25 (bottom)
// ********************************************************
void LCD_Goto(unsigned char x, unsigned char y){
  if ((x > MAX_CHARS_X ) || (y > MAX_CHARS_Y) ) return;
  cursorX = x * 6;
  cursorY = y * 9;
}

// ************** LCD_SetTextColor ************************
// - Sets the color that characters will be printed in
// R0 is r red
// R1 is g green
// R2 is b blue
// ********************************************************
void LCD_SetTextColor(unsigned char r, unsigned char g, unsigned char b){
  textColor = convertColor(r, g, b);
}

// ************** LCD_SetTextColorRGB ************************
// - Sets the color that characters will be printed in
// R0 is 16-bit color, 5-6-5 RGB
// ********************************************************
void LCD_SetTextColorRGB(unsigned short color){
  textColor = color;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                 DRAWING FUNCTIONS                                             //
///////////////////////////////////////////////////////////////////////////////////////////////////

// ************** LCD_DrawPixel ***************************
// - Draws a 16-bit pixel on the screen
// ********************************************************
void LCD_DrawPixel(unsigned short x, unsigned short y, unsigned short color)
{
    // Set the X address of the display cursor.
    LCD_WriteCommand(SSD2119_X_RAM_ADDR_REG);
    LCD_WriteData(x);

    // Set the Y address of the display cursor.
    LCD_WriteCommand(SSD2119_Y_RAM_ADDR_REG);
    LCD_WriteData(y);

    // Write the pixel value.
    LCD_WriteCommand(SSD2119_RAM_DATA_REG);
    LCD_WriteData(color);
}

// ************** LCD_DrawPixelRGB ************************
// - Draws a 16-bit representation of a 24-bit color pixel
// ********************************************************
void LCD_DrawPixelRGB(unsigned short x, unsigned short y, unsigned char r, unsigned char g, unsigned char b){
    LCD_DrawPixel(x, y, convertColor(r, g, b));
}

// ************** LCD_DrawLine ****************************
// - Draws a line using the Bresenham line algrorithm from
//   http://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm
// ********************************************************
void LCD_DrawLine(unsigned short startX, unsigned short startY, unsigned short endX, unsigned short endY, unsigned short color){
    short x0 = startX;
    short x1 = endX;
    short y0 = startY;
    short y1 = endY;

    short dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    short dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
    short err = (dx>dy ? dx : -dy)/2, e2;

    for(;;){
        LCD_DrawPixel(x0, y0, color);
        if (x0==x1 && y0==y1) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

// ************** LCD_DrawRect ****************************
// - Draws a rectangle, top left corner at (x,y)
// ********************************************************
void LCD_DrawRect(unsigned short x, unsigned short y, short width, short height, unsigned short color){
    LCD_DrawLine(x, y, x + width, y, color);
    LCD_DrawLine(x, y + 1, x, y + height - 1, color);
    LCD_DrawLine(x, y + height, x + width, y + height, color);
    LCD_DrawLine(x + width, y + 1, x + width, y + height - 1, color);
}

// ************** LCD_DrawFilledRect **********************
// - Draws a filled rectangle, top left corner at (x,y)
// ********************************************************
void LCD_DrawFilledRect(unsigned short x, unsigned short y, short width, short height, unsigned short color){
    int i, j;

    for (i = 0; i < height; i++) {
        // Set the X address of the display cursor.
        LCD_WriteCommand(SSD2119_X_RAM_ADDR_REG);
        LCD_WriteData(x);        

        // Set the Y address of the display cursor.
        LCD_WriteCommand(SSD2119_Y_RAM_ADDR_REG);
        LCD_WriteData(y + i);

        LCD_WriteCommand(SSD2119_RAM_DATA_REG);
        for (j = 0; j < width; j++) {
            LCD_WriteData(color);
        }
    }
}

// ************** LCD_DrawCircle **************************
// - Draws a circle centered at (x0, y0)
// ********************************************************
void LCD_DrawCircle(unsigned short x0, unsigned short y0, unsigned short radius, short color){    
  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;
 
  LCD_DrawPixel(x0, y0 + radius, color);
  LCD_DrawPixel(x0, y0 - radius, color);
  LCD_DrawPixel(x0 + radius, y0, color);
  LCD_DrawPixel(x0 - radius, y0, color);
 
  while(x < y)
  {
    // ddF_x == 2 * x + 1;
    // ddF_y == -2 * y;
    // f == x*x + y*y - radius*radius + 2*x - y + 1;
    if(f >= 0) 
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;    
    LCD_DrawPixel(x0 + x, y0 + y, color);
    LCD_DrawPixel(x0 - x, y0 + y, color);
    LCD_DrawPixel(x0 + x, y0 - y, color);
    LCD_DrawPixel(x0 - x, y0 - y, color);
    LCD_DrawPixel(x0 + y, y0 + x, color);
    LCD_DrawPixel(x0 - y, y0 + x, color);
    LCD_DrawPixel(x0 + y, y0 - x, color);
    LCD_DrawPixel(x0 - y, y0 - x, color);
  }
}

// ************** LCD_DrawFilledCircle ********************
// - Draws a filled circle centered at (x0, y0)
// ********************************************************
void LCD_DrawFilledCircle(unsigned short x0, unsigned short y0, unsigned short radius, short color){
    short x = radius, y = 0;
    short radiusError = 1-x;
    short i = 0;
    
    while(x >= y)
    {

        //LCD_DrawLine(x0 + x, y0 + y, x0 - x, y0 + y, color);
        for (i = x0 - x; i < x0 + x; i++){
            LCD_DrawPixel(i, y0 + y, color);
        }
        
        //LCD_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);
        for (i = x0 - x; i < x0 + x; i++){
            LCD_DrawPixel(i, y0 - y, color);
        }
        
        //LCD_DrawLine(x0 + y, y0 + x, x0 + y, y0 - x, color);
        for (i = y0 - x; i < y0 + x; i++){
            LCD_DrawPixel(x0 + y, i, color);
        }
        
        //LCD_DrawLine(x0 - y, y0 + x, x0 - y, y0 - x, color);
        for (i = y0 - x; i < y0 + x; i++){
            LCD_DrawPixel(x0 - y, i, color);
        }
        
        y++;
         
        // Calculate whether we need to move inward a pixel
        if(radiusError<0) {
            radiusError += 2*y+1;
        } else {
            x--;
            radiusError += 2*(y-x+1);
        }
    }
}

// ************** LCD_DrawImage ***************************
// - Draws an image from memory
// - Image format is a plain byte array (no metadata)
// - User must specify:
//   - pointer to image data
//   - x, y location to draw image
//   - width and height of image
//   - bpp (bits per pixel) of image
//     - currently supports 4 and 8 bpp image data
// ********************************************************
void LCD_DrawImage(const unsigned char imgPtr[], unsigned short x, unsigned short y, unsigned short width, unsigned short height, unsigned char bpp){
    short i, j, pixelCount;
    
    pixelCount = 0;
    
    for (i = 0; i < height; i++) {
        // Set the X address of the display cursor.
        LCD_WriteCommand(SSD2119_X_RAM_ADDR_REG);
        LCD_WriteData(x);        

        // Set the Y address of the display cursor.
        LCD_WriteCommand(SSD2119_Y_RAM_ADDR_REG);
        LCD_WriteData(y + i);

        LCD_WriteCommand(SSD2119_RAM_DATA_REG);
        
        switch (bpp){
            case 4:
            {
                for (j = 0; j < width/2; j++) {
                    unsigned char pixelData = imgPtr[pixelCount];
                    LCD_WriteData(CONVERT4BPP((pixelData&0xF0)>>4));            
                    LCD_WriteData(CONVERT4BPP(pixelData&0x0F));
                    pixelCount++;
                }
            }
            case 8:
            {
                for (j = 0; j < width; j++) {
                    char pixelData = *imgPtr + (i*j) + j;
                    LCD_WriteData( CONVERT8BPP(j) );
                    LCD_WriteData( CONVERT8BPP(pixelData&0x0F) );
                }
            }
        };
    }
}

// ************** LCD_DrawBMP *****************************
// - Draws an image from memory
// - Image format is a BMP image stored in a byte array
// - Function attempts to resolve the following metadata
//   from the BMP format
//   - width 
//   - height
//   - bpp
//   - location of image data within bmp data
// - User must specify:
//   - pointer to image data
//   - x, y location to draw image
// ********************************************************
void LCD_DrawBMP(const unsigned char* imgPtr, unsigned short x, unsigned short y){
    short i, j, bpp;
    long width, height, dataOffset;
    const unsigned char* pixelOffset;
    
    // read BMP metadata
    width = *(imgPtr + BMP_WIDTH_OFFSET);
    height = *(imgPtr + BMP_HEIGHT_OFFSET);
    bpp = *(imgPtr + BMP_BPP_OFFSET);
    dataOffset = *(imgPtr + BMP_DATA_OFFSET);

    // TODO : check compression type (do nothing if compresses)
    
    // debug info
//    printf("height: %d, width: %d, bpp %d", height, width, bpp);
    
    // setup pixel pointer
    pixelOffset = imgPtr + dataOffset;

    for (i = 0; i < height; i++) {
        // Set the X address of the display cursor.
        LCD_WriteCommand(SSD2119_X_RAM_ADDR_REG);
        LCD_WriteData(x);        

        // Set the Y address of the display cursor.
        LCD_WriteCommand(SSD2119_Y_RAM_ADDR_REG);
        LCD_WriteData(y + height - i);

        LCD_WriteCommand(SSD2119_RAM_DATA_REG);

        switch(bpp){
            case 1:
            {   // unknown if working yet
                for (j = 0; j < width/8; j++) {
                    unsigned char pixelData = *(pixelOffset);
                    LCD_WriteData((pixelData&0x80)*0xFFFF);            
                    LCD_WriteData((pixelData&0x40)*0xFFFF);            
                    LCD_WriteData((pixelData&0x20)*0xFFFF);            
                    LCD_WriteData((pixelData&0x10)*0xFFFF);            
                    LCD_WriteData((pixelData&0x08)*0xFFFF);            
                    LCD_WriteData((pixelData&0x04)*0xFFFF);            
                    LCD_WriteData((pixelData&0x02)*0xFFFF);            
                    LCD_WriteData((pixelData&0x01)*0xFFFF);            
                    pixelOffset++;
                }
            }
            case 4:
            {   // working?
                for (j = 0; j < width/2; j++) {
                    unsigned char pixelData = *(pixelOffset);
                    LCD_WriteData( CONVERT4BPP((pixelData&0xF0)>>4) );            
                    LCD_WriteData( CONVERT4BPP(pixelData&0x0F) );
                    pixelOffset++;
                }
            }
            case 24:
            {   // seems to work
                for (j = 0; j < width; j++) {
                    // read 24bit RGB value into pixelData
                    unsigned long pixelData = *(pixelOffset) | *(pixelOffset + 1) << 8 | *(pixelOffset + 2) << 16;
                    
                    // write RGB value to screen (passed through conversion macro)
                    LCD_WriteData( CONVERT24BPP(pixelData) );
                    
                    // increment pixel data pointer to next 24bit value
                    pixelOffset += 3;
                }                
            }
        }
    }
}
