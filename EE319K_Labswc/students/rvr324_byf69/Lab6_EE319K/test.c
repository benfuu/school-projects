#include "dac.h" 
int test(void){ 
 unsigned long Data; // 0 to 15 DAC output 
 PLL_Init(); // like Program 4.6 in the book, 80 MHz 
 DAC_Init(); 
 for(;;) { 
 DAC_Out(Data); 
 Data = 0x0F&(Data+1); // 0,1,2...,14,15,0,1,2,... 
 } 
} 