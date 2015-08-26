// Lab8.c
// Runs on LM4F120 or TM4C123
// Use the SysTick timer to request interrupts at a particular period.
// Ben Fu and Rochelle Roberts
// 4/7/14

// Analog Input connected to PE2=ADC1
// displays on Kentec EB-LM4F120-L35
// PF1 SysTick toggle


#include "lcd.h"
#include "pll.h"
#include "ADC.h"
#include "../inc/tm4c123gh6pm.h"
#include "UART.h"
#include "fifo.h"



void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
#define PF1       (*((volatile unsigned long *)0x40025008))
#define PF2       (*((volatile unsigned long *)0x40025010))
#define PF3       (*((volatile unsigned long *)0x40025020))
void PortF_Init(void){unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x00000020;   // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;         //    allow time to stabilize
  GPIO_PORTF_DIR_R |= 0x04;      // 2) make PF2 output
  GPIO_PORTF_AFSEL_R &= ~0x04;    // 3) disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;      // 4) enable digital I/O on PF2
}
void SysTick_Init(unsigned long period){
	NVIC_ST_CTRL_R =0;						//disable clock during setup
	NVIC_ST_RELOAD_R = period - 1;		//reload value so interrupt at 40Hz 
	NVIC_ST_CURRENT_R =0;					//write to clear
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF)|0x40000000; //Priotity 2
	NVIC_ST_CTRL_R = 0x00000007; 	//enable with core clock and interrupts
}

//systick handler
unsigned long ADCMail;
unsigned char ADCStatus;
unsigned long TxCounter=0;	//initialize counter


//equation is POSITION = 0.4351(DATA)+180.35
unsigned long Convert(unsigned long input){unsigned long output;
	output = (4351*input+1803500)/10000;
	//output = 0.4351*(input)+180.35;
  return output;
}

void SysTick_Handler(void){
	PF2 ^=0x04;				// 1.toggle heartbeat
	ADCMail = ADC_In();		// 2.sample ADC
	PF2 ^=0x04;				// 3.toggle heartbeat
	ADCMail = Convert(ADCMail);	//4.convert fixed point
//send the 8-byte message
	UART1_OutChar(0x02);		//send first byte (STX)
	UART1_OutChar((ADCMail/1000)+0x30);	//first digit
	UART1_OutChar(0x2E);		//.
	UART1_OutChar(((ADCMail%1000)/100)+0x30);	//second digit
	UART1_OutChar((((ADCMail%1000)%100)/10)+0x30);		//third digit
	UART1_OutChar((((ADCMail%1000)%100)%10)+0x30);		//last digit
	UART1_OutChar(0x0D);		//send CR byte
	UART1_OutChar(0x03);		//send last byte (ETX)
	TxCounter++;			// 6. Increment counter
	PF2 ^=0x04;				// 7. toggle heartbeat
	ADCStatus = 1;		//ADCStatus=1 to indicate fresh sample in ADCMail
}




unsigned long Data;      // 12-bit ADC
unsigned long Position;  // 32-bit fixed-point 0.001 cm

long letter;

int main(){unsigned char i;
	PLL_Init();				//initialize everything
	LCD_Init();
  LCD_SetTextColorRGB(YELLOW);
  PortF_Init();
	ADC_Init();
	UART1_Init();
	SysTick_Init(2000000);		//initialize interrupts
  EnableInterrupts();
	while(1){
		while(ADCStatus!=1){} //wait until fresh sample available
		ADCStatus = 0;				//clear mailbox flag
		while(Fifo_Get(&letter) == 0){};	//if the buffer is empty do nothing
		while(letter!=0x02){Fifo_Get(&letter);};		//wait until letter is 0x02
		LCD_Goto(0,0);
		for(i=0;i<5;i++){
		Fifo_Get(&letter);		//read a bit
		LCD_OutChar(letter);	//output to display
		}
		LCD_OutString(" cm  ");
		//Position = ADCMail;
		//
    //LCD_OutFix(Position); 
}
}

int main2(){
	PLL_Init();				//initialize everything
	LCD_Init();
  LCD_SetTextColorRGB(YELLOW);
  PortF_Init();
	ADC_Init();
	UART1_Init();
	SysTick_Init(2000000);
  EnableInterrupts();

	while(1){
		while(ADCStatus!=1){} //wait until fresh sample available
		ADCStatus = 0;				//clear mailbox flag
		Position = ADCMail;
		LCD_Goto(0,0);
    LCD_OutFix(Position); LCD_OutString(" cm  ");
}
}

//tester
int Status[20]; // entries 0,7,12,19 should be false, others true 
long GetData[10]; // entries 1 2 3 4 5 6 7 8 should be 1 2 3 4 5 6 7 8 
int main3(void){ 
 Fifo_Init(); 
 for(;;){ 
 Status[0] = Fifo_Get(&GetData[0]); // should fail, empty 
 Status[1] = Fifo_Put(1); // should succeed, 1 
 Status[2] = Fifo_Put(2); // should succeed, 1 2 
 Status[3] = Fifo_Put(3); // should succeed, 1 2 3 
Status[4] = Fifo_Put(4); // should succeed, 1 2 3 4 
 Status[5] = Fifo_Put(5); // should succeed, 1 2 3 4 5 
 Status[6] = Fifo_Put(6); // should succeed, 1 2 3 4 5 6 
 Status[7] = Fifo_Put(7); // should fail, 1 2 3 4 5 6 
 Status[8] = Fifo_Get(&GetData[1]); // should succeed, 2 3 4 5 6 
 Status[9] = Fifo_Get(&GetData[2]); // should succeed, 3 4 5 6 
 Status[10] = Fifo_Put(7); // should succeed, 3 4 5 6 7 
 Status[11] = Fifo_Put(8); // should succeed, 3 4 5 6 7 8 
 Status[12] = Fifo_Put(9); // should fail, 3 4 5 6 7 8 
 Status[13] = Fifo_Get(&GetData[3]); // should succeed, 4 5 6 7 8 
 Status[14] = Fifo_Get(&GetData[4]); // should succeed, 5 6 7 8 
 Status[15] = Fifo_Get(&GetData[5]); // should succeed, 6 7 8 
 Status[16] = Fifo_Get(&GetData[6]); // should succeed, 7 8 
 Status[17] = Fifo_Get(&GetData[7]); // should succeed, 8 
 Status[18] = Fifo_Get(&GetData[8]); // should succeed, empty 
 Status[19] = Fifo_Get(&GetData[9]); // should fail, empty 
 } 
}



