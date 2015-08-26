// Lab10 Arcade Style Game
// main.c
// Runs on LM4F120 or TM4C123
// Put your name here or look very silly
// Put the date here or look very silly

// Graphic display on Kentec EB-LM4F120-L35
// Sound output to DAC (Lab 7)
// Analog Input connected to PE2=ADC1, 1-D joystick (Lab 8)
// optional: UART link to second board for two-player mode
// Switch input

#include "../inc/tm4c123gh6pm.h"
#include "SSD2119.h"
#include "PLL.h"
#include "random.h"
#include "SpaceArt.h"     // space invader art
#include "Sound.h"				//DAC sounds
#include "Piano.h"				//piano switches
#include "dac.h"				//DAC stuff
#include "Pokemon.h"		//pokemon
#include "PokemonSounds.h"	//pokemon sounds


void EnableInterrupts(void);
void DisableInterrupts(void);
void Timer2_Init(unsigned long period);
unsigned long TimerCount;
unsigned long Semaphore;

unsigned long ADCMail;
unsigned char ADCStatus;

unsigned long hits;				//number of hits
unsigned long misses;			//number of misses
unsigned long kill;				//enemy got hit
unsigned long Pokeballs;		//pokeballs left
unsigned long Accuracy;			//accuracy

//ADC data and Ship position stuff
unsigned long NewData;      // 12-bit ADC
unsigned long OldData;			//old data
unsigned long Ship_Position;  // 32-bit fixed-point pixel

struct State {
  unsigned long x;      // x coordinate
  unsigned long y;      // y coordinate
  const unsigned char *image; // ptr->image
  long life;            // 0=dead, 50=max health
	unsigned long border;		//border switch flag
};
typedef struct State STyp;


//missles (6)
STyp Missile[1];
void Missile_Init(void){int i;
	for(i=0;i<1;i++){
		Missile[i] .x = 0;
		Missile[i] .y = 208;
		Missile[i] .image = pokeball;
		Missile[i] .life = 0;
	}
}
void Missle_Move(void){int i;
	for(i=0;i<1;i++){
		if(Missile[i].life==1){
		Missile[i].y -= 4;				//shoot upwards
	}	else{
		Missile[i].y = 19;
	}
}
}

//enemy 1 (4)
STyp Enemy1[4];
void Enemy1_Init(void){int i;
  for(i=0;i<4;i++){
    Enemy1[i].x = 40*i;
    Enemy1[i].y = 20+(32*i);
    Enemy1[0].image = pikachu;
		Enemy1[1].image = bulbasaur;
		Enemy1[2].image = charmander;
		Enemy1[3].image = squirtle;
    Enemy1[i].life = 10;
		Enemy1[i].border=0;				//move right
   }
}
void Enemy1_Move(void){int i;
	for(i=0;i<4;i++){
		if(Enemy1[i].life==0){
			Enemy1[i].image=black;
			Enemy1[i].x=0;
		}
		if(Enemy1[i].x>=287){
			Enemy1[i].border=1;
		}
		if(Enemy1[i].x==0){
			Enemy1[i].border=0;
		}
		if(Enemy1[i].border==0){									//forwards
			if(Enemy1[i].x < 288){
				Enemy1[i].x += 1;
			}
		}
		if(Enemy1[i].border==1){									//backwards
			if(Enemy1[i].x > 0){
				Enemy1[i].x -= 1;
			}
		}
	}
}

void PortE_Init(void){unsigned long volatile delay;
	//set PE0 for switch input
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE; // activate port E
  delay = SYSCTL_RCGC2_R;    			// 2 NOP
	GPIO_PORTE_AMSEL_R &= ~0x01;     	// no analog
  GPIO_PORTE_PCTL_R &= ~0x0000000F;	// regular GPIO function
  GPIO_PORTE_DIR_R &= ~0x01;      // PE0 is input
  GPIO_PORTE_AFSEL_R &= ~0x01;   // disable alternate select function on PE0
  GPIO_PORTE_DEN_R |= 0x01;      // dig enable for PE0
	//set PE2 for ADC input
  GPIO_PORTE_DIR_R &= ~0x04;      // 2) make PE2 input
  GPIO_PORTE_AFSEL_R |= 0x04;     // 3) enable alt funct on PE2
  GPIO_PORTE_DEN_R &= ~0x04;      // 4) disable digital I/O on PE2
  GPIO_PORTE_AMSEL_R |= 0x04;     // 5) enable analog funct on PE2
}

void SysTick_Init(unsigned long period){
	NVIC_ST_CTRL_R =0;						//disable clock during setup
	NVIC_ST_RELOAD_R = period - 1;		//reload value so interrupt at 40Hz 
	NVIC_ST_CURRENT_R =0;					//write to clear
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF)|0x40000000; //Priotity 2
	NVIC_ST_CTRL_R = 0x00000007; 	//enable with core clock and interrupts
}

// You can use this timer only if you learn how it works
void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  Semaphore = 0;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  //TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}
void Timer2A_Stop(void){
  TIMER2_CTL_R &= ~0x00000001; // disable
}
void Timer2A_Start(void){
  TIMER2_CTL_R |= 0x00000001;   // enable
}


//CONVERT FUNCTION
unsigned long ConvertX(unsigned long input){unsigned long output;
	output = (288*input)/4096;
  return output;
}

void delay_long(void){unsigned long i;
	for(i=0;i<30000000;i++){}
	}
void delay_short(void){unsigned long i;
	for(i=0;i<16000000;i++){}
	}
void delay_blink(void){unsigned long i;
	for(i=0;i<9000000;i++){}
	}
//PLAY FUNCTION
unsigned char anysound[10000];			//global array for all sounds
unsigned long int Sound;
unsigned long int Sound_Count;
void Sound_Play(const unsigned char x[], unsigned long int number){unsigned long int i; unsigned long int j;
	//clear sound array
	for(i=0;i<10000;i++){
		anysound[i] = 0;
	}
	for(j=0;j<number;j++){
		anysound[j] = x[j];
	}
	Sound_Count = number;			//Sound_Count is the number of entries in the sound array
	Timer2A_Start();				//start the sound
}

//GAME
int main(void){unsigned long i=64,j=0;
  PLL_Init();  // Set the clocking to run at 80MHz from the PLL.
  LCD_Init();  // Initialize LCD
	Piano_Init();
	Sound_Init();
	ADC_Init();		//initialize ADC
	ADC_SetChannel(1); //PE2
	
	Missile_Init();
	Enemy1_Init();
	
  LCD_Goto(10,0);
  LCD_SetTextColor(255,0,0); // yellow= red+green, no blue
  printf("Welcome to Pokemon Capture!");
	delay_long();
	LCD_DrawFilledRect(0,0,320,80,0x00);
	delay_short();
	
	while((GPIO_PORTE_DATA_R&0x01)==0){
		LCD_Goto(10,0);
		printf("Press any button to begin...");
		delay_blink();
		LCD_DrawFilledRect(0,0,320,80,0x00);
		delay_blink();
	}
	delay_short();
	
	LCD_SetTextColor(255,255,0);
	LCD_Goto(0,0);
	printf("Tragedy has struck Pallet Town! Four Pokemon have    escaped from their Pokeballs, and it's your job to   put them back where they belong.");
	delay_long();
	delay_long();
	delay_long();
	LCD_DrawFilledRect(0,0,320,80,0x00);
	delay_short();
	
	LCD_Goto(0,0);
	printf("Use the slider to aim, and when you think you have a shot, press the button to throw your Pokeball.");
	delay_long();
	delay_long();
	delay_long();
	LCD_DrawFilledRect(0,0,320,80,0x00);
	delay_blink();
	
	LCD_Goto(0,0);
	printf("You only have so many Pokeballs, so be smart where   you throw them!");
	delay_long();
	delay_long();
	LCD_DrawFilledRect(0,0,320,80,0x00);
	delay_short();
	
	LCD_Goto(0,0);
	printf("Good luck!");
	delay_long();
	LCD_DrawFilledRect(0,0,320,80,0x00);
	delay_short();
	
	LCD_SetTextColor(255,0,0);
	LCD_Goto(0,0);
	printf("Captured:");
	
	LCD_SetTextColor(0,255,0);
	LCD_Goto(14,0);
	printf("Pokeballs Left:");
	
	LCD_SetTextColor(0,0,255);
	LCD_Goto(35,0);
	printf("Accuracy:");
  LCD_DrawLine(10,16,310,16,BURNTORANGE);
  
  Timer2_Init(7256); // 11kHz
	SysTick_Init(2666667);	//30 Hz
  EnableInterrupts();
	
	Pokeballs = 25;
  while(1){
		if((25-(hits+misses))==0){
			if(kill!=4){
				DisableInterrupts();
				LCD_SetTextColor(255,0,0);
				
				LCD_DrawFilledRect(0,0,320,80,0x00);
				delay_short();
				
				LCD_Goto(0,0);
				printf("Sorry, you were not able to catch all the Pokemon.");
				delay_long();
				delay_long();
				LCD_DrawFilledRect(0,0,320,80,0x00);
				delay_short();
				
				LCD_Goto(0,0);
				printf("Try again.");
				delay_long();
				LCD_DrawFilledRect(0,0,320,80,0x00);
				delay_short();
				
				while(1){
				LCD_Goto(0,0);
				printf("Press RESET to play again.");
				delay_blink();
				LCD_DrawFilledRect(0,0,320,80,0x00);
				delay_blink();
				}
			}
		}
				
		if(kill==4){
			DisableInterrupts();
			LCD_SetTextColor(0,255,255);
			
			LCD_DrawFilledRect(0,0,320,80,0x00);
			delay_short();
			
			LCD_Goto(0,0);
			printf("Congratulations! You have captured all the Pokemon!");
			delay_long();
			delay_long();
			LCD_DrawFilledRect(0,0,320,80,0x00);
			delay_short();
			
			LCD_Goto(0,0);
			printf("You are now a Pokemon Master!");
			delay_long();
			delay_long();
			LCD_DrawFilledRect(0,0,320,80,0x00);
			delay_short();
			
			while(1){
				LCD_Goto(0,0);
				printf("Press RESET to play again.");
				delay_blink();
				LCD_DrawFilledRect(0,0,320,80,0x00);
				delay_blink();
			}
		}
		
		
    if(Semaphore){
			
			LCD_SetTextColor(255,0,0);
			LCD_Goto(10,0);
			LCD_PrintInteger(kill);
			
			LCD_SetTextColor(0,255,0);
			LCD_Goto(30,0);
			LCD_PrintInteger(25-(hits+misses));
			if((25-(hits+misses))<10){
				LCD_Goto(31,0);
				printf(" ");
			}
			
			LCD_SetTextColor(0,0,255);
			LCD_Goto(45,0);
			LCD_PrintInteger(hits/(hits+misses));
			
			//draw ship
			if(Ship_Position>288){
				Ship_Position=288;
			}
      LCD_DrawBMP(PlayerShip,Ship_Position,220);
			LCD_DrawFilledRect(0,220,Ship_Position,9,0x00);
			LCD_DrawFilledRect(Ship_Position+32,220,320-(Ship_Position+32),9,0x00);
			
			
      for(j=0;j<1;j++){
				if(Missile[j].life==1){
					LCD_DrawBMP(Missile[j].image, Missile[j].x, Missile[j].y);
				} else{
					LCD_DrawFilledRect(Missile[j].x,Missile[j].y,16,18,0x00);
				}
				if(Missile[j].y<=18){
					LCD_DrawFilledRect(Missile[j].x,Missile[j].y,16,18,0x00);
					Missile[j].life=0;
					misses++;
				}
			}
			
      for(i=0;i<4;i++){
				if(Enemy1[i].life!=0){
					LCD_DrawBMP(Enemy1[i].image,Enemy1[i].x,Enemy1[i].y);
				}
				else{
					LCD_DrawFilledRect(Enemy1[i].x,Enemy1[i].y,32,32,0x00);
				}
			}
			
			LCD_DrawFilledRect(0,17,320,4,0x00);
			
      Semaphore = 0;
    }
  }
}

void SysTick_Handler(void){static unsigned char i=0; unsigned char x; unsigned char y;
//adc stuff
	ADCMail = ADC_Read();	//call ADC_In to get sample and put into global variable, ADCMail
	NewData = ADCMail;
	if(NewData!=OldData){
		Ship_Position = ConvertX(NewData);	//get the converted position into pixels
		OldData = NewData;			//update the new data
	}	else{
		Ship_Position = ConvertX(OldData);	//not new, so get old data into pixels
	}
//switch stuff
	if((GPIO_PORTE_DATA_R&0x01)==1){		//if switch is pressed
		//fire missile
		if(x!=0){
			if(Missile[i].life==0){				//if missile doesn't exist then create it
				Missile[i] .x = (Ship_Position+(Ship_Position+28))/2;
				Missile[i] .y = 208;		//top of the ship
				Missile[i] .life = 1;			//give the missile life
				//sounds
				Sound_Play(Pokeball_Sound,4080);
				x=0;
			}
		}
		i = (i+1)%1;
		Missle_Move();
	}	else{
		Missle_Move();
		//dontfiremissle
	}
//move sprites
	Enemy1_Move();
	
//test for hit
	for(y=0;y<4;y++){
		for(x=0;x<1;x++){
			if((Missile[x].x+16)>=Enemy1[y].x){
				if(Missile[x].x<=(Enemy1[y].x+33)){
					if(Missile[x].y<=(Enemy1[y].y+32)){
						if(Missile[x].y>=Enemy1[y].y){
							if(Missile[x].life==1){
								if(Enemy1[y].life>0){
									Missile[x].life=0;
									Enemy1[y].life-=2;
									hits++;
									if(Enemy1[y].life==0){
										Sound_Play(Pikachu_Sound,10000);
										kill++;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	ADCStatus = 1;			//ADCStatus=1 to indicate fresh sample in ADCMail
	Semaphore = 1; 			// trigger
}


void Timer2A_Handler(void){static unsigned long i= 0;
  TIMER2_ICR_R = 0x00000001;   // acknowledge timer2A timeout
	
	//output to DAC
	DAC_Out(anysound[i]);
	i = (i+1)%Sound_Count;
	if(i==0){
		Timer2A_Stop();
	}
//other conter stuff
  TimerCount++;
}


