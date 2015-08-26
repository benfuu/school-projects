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
unsigned long ADCMail;
unsigned char ADCStatus;
void SysTick_Handler(void){
	PF2 ^=0x04;	//Toggle Heartbeat PF2	
	ADCMail = ADC_In();	//call ADC_In to get sample and put into global variable, ADCMail
	ADCStatus = 1;			//ADCStatus=1 to indicate fresh sample in ADCMail
}
unsigned long Data;      // 12-bit ADC
unsigned long Position;  // 32-bit fixed-point 0.001 cm
	
int main1(void){      // single step this program and look at Data
  PLL_Init();         // Bus clock is 80 MHz 
  ADC_Init();         // turn on ADC, set channel to 1
  while(1){                
    Data = ADC_In();  // sample 12-bit channel 1
  }
}

int main2(void){
  PLL_Init();         // Bus clock is 80 MHz 
  ADC_Init();         // turn on ADC, set channel to 1
  LCD_Init();   
  LCD_SetTextColorRGB(YELLOW);
  PortF_Init();
  while(1){           // use scope to measure execution time for ADC_In and LCD_OutDec           
    PF2 = 0x04;       // Profile
    Data = ADC_In();  // sample 12-bit channel 1
    PF2 = 0x00;       // Profile
    LCD_Goto(0,0);
    LCD_OutDec(Data); LCD_OutString("    "); 
  }
}

//equation is POSITION = 0.4351(DATA)+180.35
unsigned long Convert(unsigned long input){unsigned long output;
	output = (4351*input+1803500)/10000;
  return output;
}
int main(void){ 
  PLL_Init();        // Bus clock is 80 MHz 
  LCD_Init();   
  LCD_SetTextColorRGB(YELLOW);
  PortF_Init();
  ADC_Init();        // turn on ADC, set channel to 1
  while(1){  
    PF2 ^= 0x04;     // Heartbeat
    Data = ADC_In(); // sample 12-bit channel 1
    Position = Convert(Data); 
    LCD_Goto(0,0);
    LCD_OutDec(Data); LCD_OutString("    "); 
    LCD_OutFix(Position); LCD_OutString(" cm  ");
  }
}   

int main4(){
	PLL_Init();				//initialize everything
	LCD_Init();
  LCD_SetTextColorRGB(YELLOW);
  PortF_Init();
	ADC_Init();
	SysTick_Init(2000000);
  EnableInterrupts();
	while(1){
		while(ADCStatus!=1){} //wait until fresh sample available
		Data = ADCMail;
		ADCStatus = 0;				//clear mailbox flag
		Position = Convert(Data);
		LCD_Goto(0,0);
    //LCD_OutDec(Data); LCD_OutString("    ");  DON'T NEED THIS 
    LCD_OutFix(Position); LCD_OutString(" cm  ");
}
}


