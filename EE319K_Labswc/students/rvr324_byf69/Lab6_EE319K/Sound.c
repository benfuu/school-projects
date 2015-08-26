// Sound.c, 
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 8/25/2013 
// Last Modified: 10/9/2013 
// Section 1-2pm     TA: Saugata Bhattacharyya
// Lab number: 6
// Hardware connections
#include "tm4c123gh6pm.h"
#include "dac.h"
#include "piano.h"
// put code definitions for the software (actual C code)
// this file explains how the module works

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Input: Initial interrupt period
//           Units to be determined by YOU
//           Maximum to be determined by YOU
//           Minimum to be determined by YOU
// Output: none
const unsigned char SineWave[32] = {8,9,11,12,13,14,14,15,15,15,14,14,13,12,11,9,8,7,5,4,3,2,2,1,1,1,2,2,3,4,5,7};
static int Index=0;           // Index varies from 0 to 31
void Sound_Init(unsigned long period){unsigned volatile long delay;
  DAC_Init();          // Port E is DAC
  Index = 0;						// initialize the Index pointer
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // activate port B
  delay = SYSCTL_RCGC2_R;    			// 2 NOP
  GPIO_PORTB_DIR_R |= 0x04;      // PB2 is output (heartbeat)
  GPIO_PORTB_AFSEL_R &= ~0x04;   // disable alternate select function on PB2
  GPIO_PORTB_DEN_R |= 0x04;      // dig enable for PB2
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF); // priority 0
  NVIC_ST_CTRL_R = 0x00000007; // enable SysTick with core clock and interrupts
}

// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Input: interrupt period
//           Units to be determined by YOU
//           Maximum to be determined by YOU
//           Minimum to be determined by YOU
//         input of zero disables sound output
// Output: none

void Sound_Play (unsigned long volatile note){
	switch (note){
		case 0x01: NVIC_ST_RELOAD_R = 4778;
			break;
		case 0x02: NVIC_ST_RELOAD_R = 4257;
			break;
		case 0x04: NVIC_ST_RELOAD_R = 3792;
			break;
		case 0x05: Song();
			break;
		default: NVIC_ST_RELOAD_R = 0;
	}
}
void delay_note(void){unsigned long i; unsigned char x;
	for(i=0;i<2000000;i++){
		x=1;
	}
		GPIO_PORTF_DATA_R ^= 0x04; //toggle PF2 for heartbeat
}	
void delay_rest(void){unsigned long i; unsigned char x;
	for(i=0;i<50000;i++){
		x=1;
	}
		GPIO_PORTF_DATA_R ^= 0x04; //toggle PF2 for heartbeat
}	
int Song(void){unsigned long i; //play the Hobbit
	const unsigned long Song[98] = {4778,4257,3792,3792,3792,3189,3189,3189,3792,3792,3792,4257,4257,4257,4778,4778,
		4778,4778,4778,4778,4778,4778,4778,3792,3792,3189,3189,2841,2841,2841,2841,2841,2389,2389,2531,2531,2531,3189,
		3189,3189,3792,3792,3792,3792,3792,3579,3792,4257,4257,4257,4257,4257,4257,
		4778,4257,3792,3792,3792,3189,3189,3189,3792,3792,3792,4257,4257,4257,4778,4257,4778,4778,4778,4778,4778,4778,
		4778,3792,3792,3189,3189,2841,2841,2841,2841,2841,2841,3189,3189,3792,3792,3792,4257,4257,4257,4257,4257,4257,4257,};
	for(i=0;i<98;i++){
		NVIC_ST_RELOAD_R = Song[i];
		delay_note();
		if(
			Piano_In()==1){				//if switch one is pressed, exit
				return 0;}
		
	}
		return 0;
}


// Interrupt service routine
// Executed periodically, the actual period
// determined by the current Reload.
void SysTick_Handler(void){
	GPIO_PORTB_DATA_R ^= 0x04;		//toggle PB2 for heartbeat
  Index = (Index+1)&0x1F;    		  // increment the pointer to the index
  DAC_Out(SineWave[Index]);    // output one value each interrupt
}
