// Piano.c
// This software configures the off-board piano keys
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 8/25/2013 
// Last Modified: 10/9/2013 
// Section 1-2pm     TA: Saugata Bhattacharyya
// Lab number: 6
// Hardware connections

#include "tm4c123gh6pm.h"
// put code definitions for the software (actual C code)
// this file explains how the module works

// **************Piano_Init*********************
// Initialize piano key inputs, called once 
// Input: none 
// Output: none
void Piano_Init(void){unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x01; // activate port A
  delay = SYSCTL_RCGC2_R;    			// 2 NOP
	GPIO_PORTA_AMSEL_R &= ~0x1C;     	// no analog
  GPIO_PORTA_PCTL_R &= ~0x000FFF00;	// regular GPIO function
  GPIO_PORTA_DIR_R |= ~0x1C;      // PA4-2 are inputs
  GPIO_PORTA_AFSEL_R &= ~0x1C;   // disable alternate select function on PA4-2
  GPIO_PORTA_DEN_R |= 0x1C;      // dig enable for PA4-2
}
// **************Piano_In*********************
// Input from piano key inputs 
// Input: none 
// Output: 0 to 7 depending on keys
// 0x01 is just Key0, 0x02 is just Key1, 0x04 is just Key2
unsigned long Piano_In(void){
	unsigned long volatile key;						//define variable 'key'
		key = GPIO_PORTA_DATA_R>>2;	//read the input from switches
  
   return key;  // replace this line with actual code
}
