//UART.c
// UART1_Int Transmitter
// Initialize UART1 Transmitter and set Baud Rate at 100,000 bps
//	call once

#include "../inc/tm4c123gh6pm.h"
#include "ADC.h"
#include "fifo.h"
#define PF1       (*((volatile unsigned long *)0x40025008))
#define PF2       (*((volatile unsigned long *)0x40025010))
#define PF3       (*((volatile unsigned long *)0x40025020))
	
unsigned long error;
unsigned long RxCounter;
	
void UART1_Init(void){
	Fifo_Init();
	SYSCTL_RCGC1_R |= 0x0002;			//activate UART1
	SYSCTL_RCGC2_R |= 0x0004;			//activate PortC
	UART1_CTL_R &= ~0x001;				//disable UART during init
	UART1_IBRD_R = 50;						//bit rate=(80000000/16*100000)=50
	UART1_FBRD_R = 0;
	UART1_LCRH_R = 0x0070;				//8-bit word length, enable FIFO
	//enable interrups
	UART1_IM_R |= 0x10;						//set RXIM interrupt mask
	UART1_IFLS_R &= 0xFFFFFFD7;		//clear bits 5 and 3 of IFLS
	UART1_IFLS_R |= 0x10;					//set bit 4 to enable interrupts
	//set interrupr priority
	NVIC_PRI1_R = (NVIC_PRI1_R & 0xFF00FFFF)|0x00400000; //Priotity 2
	NVIC_EN0_R |= 0x40;						//priority for UART1
	//enable UART
	UART1_CTL_R = 0x0301;					//enable RXE, TXE and UART
	GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF)+0x00220000;	//write 2
	GPIO_PORTC_AMSEL_R &= ~0x30;	//disable PC4,5 analog funtion
	GPIO_PORTC_AFSEL_R |= 0x30;		//enable PC4,5 alt function
	GPIO_PORTC_DEN_R |= 0x30;			//enable PC4,5 digital I/O
}



// UART1_OutChar
// send 1-byte info,busy-wait synchronization
void UART1_OutChar(unsigned char data){
	while((UART1_FR_R&0x0020)!=0){}; //wait until TXFF=0
	UART1_DR_R=data;	//write data
}


void UART1_Handler(void){unsigned char read; unsigned char i;
	Fifo_Init();
	PF2 ^=0x04;				//toggle heartbeat twice
	PF2 ^=0x04;
	while((UART1_FR_R&0x0010)!=0){}; //wait until RXFE is 0
	for(i=0;i<8;i++){
		read = UART1_DR_R&0xFF;		//get byte information into read
		if(Fifo_Put(read)==0)
			error++;
	}
	RxCounter++;
	UART1_ICR_R=0x10; //clear RXRIS in the RIS register
	PF2 ^=0x04;
}
