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
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE; // activate port E
  delay = SYSCTL_RCGC2_R;    			// 2 NOP
	GPIO_PORTE_AMSEL_R &= ~0x01;     	// no analog
  GPIO_PORTE_PCTL_R &= ~0x0000000F;	// regular GPIO function
  GPIO_PORTE_DIR_R &= ~0x01;      // PE0 is input
  GPIO_PORTE_AFSEL_R &= ~0x01;   // disable alternate select function on PE0
  GPIO_PORTE_DEN_R |= 0x01;      // dig enable for PE0
}
// **************Piano_In*********************
// Input from piano key inputs 
// Input: none 
// Output: 0 to 7 depending on keys
// 0x01 is just Key0, 0x02 is just Key1, 0x04 is just Key2
unsigned long Piano_In(void){
	unsigned long volatile key;						//define variable 'key'
	key = GPIO_PORTE_DATA_R;	//read the input from switches
  return key;  // replace this line with actual code
}
