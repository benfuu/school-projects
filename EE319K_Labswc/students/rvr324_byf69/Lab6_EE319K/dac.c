// dac.c
// This software configures DAC output
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

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){unsigned long volatile delay;
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE; // activate port E
  delay = SYSCTL_RCGC2_R;    			// 2 NOP
	GPIO_PORTE_AMSEL_R &= ~0x0F;     	// no analog
  GPIO_PORTE_PCTL_R &= ~0x0000FFFF;	// regular GPIO function
  GPIO_PORTE_DIR_R |= 0x0F;      // PE3-0 are outputs
  GPIO_PORTE_AFSEL_R &= ~0x0F;   // disable alternate select function on PE3-0
  GPIO_PORTE_DEN_R |= 0x0F;      // dig enable for PE3-0
	GPIO_PORTE_DR8R_R |= 0x0F;			//drive speakers
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
  GPIO_PORTE_DATA_R = data;
}
