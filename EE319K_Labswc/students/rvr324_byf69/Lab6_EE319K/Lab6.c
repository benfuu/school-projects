// Lab6.c
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 8/25/2013 
// Last Modified: 10/9/2013 
// Section 1-2pm     TA: Saugata Bhattacharyya
// Lab number: 6
// Brief description of the program
//   A digital piano with 4 keys and a 4-bit DAC
// Hardware connections


#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "Sound.h"
#include "Piano.h"
#include "timer.c"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void PortF_Init(void){unsigned long volatile delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // activate port F
  delay = SYSCTL_RCGC2_R;    			// 2 NOP
  GPIO_PORTF_DIR_R |= 0x04;      // PF2 is output
  GPIO_PORTF_AFSEL_R &= ~0x04;   // disable alternate select function on PF2
  GPIO_PORTF_DEN_R |= 0x04;      // dig enable for PF2
}

void delay_funct(void){unsigned long i; unsigned char x;
	for(i=0;i<800000;i++){
		x=1;
	}
		GPIO_PORTF_DATA_R ^= 0x04; //toggle PF2 for heartbeat
}	

int main(void){
  PLL_Init();          // bus clock at 80 MHz
  // all initializations go here
	PortF_Init();				//initialize Port F
	Piano_Init();       // Port A contains value from switches
	Sound_Init(4000);   // initialize SysTick timer
  EnableInterrupts();
  while(1){
		delay_funct();		//portF heartbeat
		Sound_Play(Piano_In());		//play the desired note
		
// main loop, read from switchs change sounds	
  }             
}


