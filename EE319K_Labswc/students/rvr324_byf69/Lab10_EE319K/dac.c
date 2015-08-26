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
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC; // activate port C
  delay = SYSCTL_RCGC2_R;    			// 2 NOP
	GPIO_PORTC_AMSEL_R &= ~0xF0;     	// no analog
  GPIO_PORTC_PCTL_R &= ~0xFFFF0000;	// regular GPIO function
  GPIO_PORTC_DIR_R |= 0xF0;      // PC7-4 are outputs
  GPIO_PORTC_AFSEL_R &= ~0xF0;   // disable alternate select function on PC7-4
  GPIO_PORTC_DEN_R |= 0xF0;      // dig enable for PC7-4
	GPIO_PORTC_DR8R_R |= 0xF0;			//drive speakers
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
  GPIO_PORTC_DATA_R = data<<4;
}
