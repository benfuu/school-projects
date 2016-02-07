/*
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Eugene Choe
    Name 2: Ben Fu
    UTEID 1: ec32934
    UTEID 2: byf69
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

int registers[8] = {0}; /* array that holds all register values */
int MAX_INST_LENGTH = 8;




/***************************************************************/
/* This function updates the latches                                                                                                                                                                                                                                          
   This function uses the following variables:                                                                                                                                                                                                                                
*/
/***************************************************************/

void update_latches(){
  CURRENT_LATCHES.PC = NEXT_LATCHES.PC;
  CURRENT_LATCHES.Z = NEXT_LATCHES.Z;
  CURRENT_LATCHES.N = NEXT_LATCHES.N;
  CURRENT_LATCHES.P = NEXT_LATCHES.P;
  return;
}




/***************************************************************/
/* This function will execute the instruction
 This function uses the following variables
 */
/***************************************************************/

void execute(int line, char* instruction, int arg1, int arg2, int arg3){
    /* add instruction , arg1 = dr, arg2 = sr1, arg3 = sr2/imm5*/
    if(strcmp(instruction, "ADD") == 0){
        NEXT_LATCHES.REGS[arg1] = arg2 + arg3;
        if(registers[arg1] < 0){
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
        else if(NEXT_LATCHES.REGS[arg1] == 0){
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        else if(NEXT_LATCHES.REGS[arg1] > 0){
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
        update_latches();
    }
    /* and instruction */
    else if(strcmp(instruction , "AND") == 0){
        NEXT_LATCHES.REGS[arg1] = (arg2 & arg3);
        if(NEXT_LATCHES.REGS[arg1] < 0){
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
        else if(NEXT_LATCHES.REGS[arg1] == 0){
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        else if(NEXT_LATCHES.REGS[arg1] > 0){
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
        update_latches();
    }
    /* jmp instruction */
    else if(strcmp(instruction , "JMP") == 0){
        NEXT_LATCHES.PC = arg1;
        update_latches();
    }
    /* ret instruction */
    else if(strcmp(instruction , "RET") == 0){
        NEXT_LATCHES.PC = NEXT_LATCHES.REGS[7]; /* previous pc should have been saved in r7 */
        update_latches();
    }
    /* jsr instruction */
    else if(strcmp(instruction , "JSR") == 0){
        CURRENT_LATCHES.PC = NEXT_LATCHES.REGS[7] + arg1;
        update_latches();
    }
    /* jsrr instruction */
    else if(strcmp(instruction , "JSRR") == 0){
        CURRENT_LATCHES.PC = arg1;
        update_latches();
    }
    /* nop instruction */
    else if(strcmp(instruction, "NOP") == 0){
        update_latches();
    }
    /* ldb instruction */
    else if(strcmp(instruction, "LDB") == 0){
        if((arg3 & 0x0020) == 0x0020){
            arg3 = arg3 | 0xFFC0; /*sign extend 1 */
        }
        else{
            arg3 = arg3 & 0x003F; /* sign extend 0 */
        }
        arg2 += arg3;
        registers[arg1] = MEMORY[arg2][1];
        if((arg1 & 0x0080) == 0x0080){
            NEXT_LATCHES.REGS[arg1] = NEXT_LATCHES.REGS[arg1] | 0xFF00; /* sign extend 1 */
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
        else{
            NEXT_LATCHES.REGS[arg1] = NEXT_LATCHES.REGS[arg1] & 0x00FF; /* sign extend 0 */
            if((NEXT_LATCHES.REGS[arg1] & 0x00FF) == 0){
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 1;
                NEXT_LATCHES.P = 0;
            }
            else{
                NEXT_LATCHES.REGS[arg1] = NEXT_LATCHES.REGS[arg1] & 0x00FF;
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.Z = 0;
                NEXT_LATCHES.P = 1;
            }
        }
    }
    /* ldw instruction */
    else if(strcmp(instruction, "LDW") == 0){
        if((arg3 & 0x0020) == 0x0020){
            arg3 = arg3 | 0xFFC0; /*sign extend 1 */
        }
        else{
            arg3 = arg3 & 0x003F; /* sign extend 0 */
        }
        arg2 += arg3;
        NEXT_LATCHES.REGS[arg1] = (MEMORY[arg2][0]) + (MEMORY[arg2][1] << 8);
        if((NEXT_LATCHES.REGS[arg1] & 0x8000) == 0x8000){
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
        else if(NEXT_LATCHES.REGS[arg1] == 0){
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        else{
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
    }
    /* lea instruction */
    else if(strcmp(instruction, "LEA") == 0){
        if((arg2 & 0x0100) == 0x0100){
	  arg2 = (arg2 | 0xFE00); /* sign extend 1 */
        }
        else{
	  arg2 = (arg2 & 0x01FF); /* sign extend 0 */
        }
	arg2 = arg2 * 2;
        NEXT_LATCHES.REGS[arg1] = CURRENT_LATCHES.PC + arg2;
        update_latches();
    }
    /* not instruction */
    else if(strcmp(instruction, "NOT") == 0){
        NEXT_LATCHES.REGS[arg1] = ~arg2;
        if(NEXT_LATCHES.REGS[arg1] < 0){
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
        else if(NEXT_LATCHES.REGS[arg1] > 0){
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
        else{
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        update_latches();
    }
    /* xor instruction */
    else if(strcmp(instruction, "XOR") == 0){
        NEXT_LATCHES.REGS[arg1] = arg2 ^ arg3;
        if(NEXT_LATCHES.REGS[arg1] < 0){
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
        else if(NEXT_LATCHES.REGS[arg1] == 0){
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        else{
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
        update_latches();
    }
    /* rshfl instruction */
    else if(strcmp(instruction, "LSHF") == 0){
        NEXT_LATCHES.REGS[arg1] = arg2 << arg3;
        if(NEXT_LATCHES.REGS[arg1] < 0){
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
        else if(NEXT_LATCHES.REGS[arg1] == 0){
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        else{
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
        update_latches();
    }
    else if(strcmp(instruction, "RSHFL") == 0){
        NEXT_LATCHES.REGS[arg1] = arg2 >> arg3;
        if(NEXT_LATCHES.REGS[arg1] < 0){
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
        else if(NEXT_LATCHES.REGS[arg1] == 0){
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        else{
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
        update_latches();
    }
    else if(strcmp(instruction, "RSHFA") == 0){
        NEXT_LATCHES.REGS[arg1] = arg2 << arg3;
        NEXT_LATCHES.REGS[arg1] += ((arg2 & 0x8000) >> 15); /* bit 15 of arg2 goes into bit 0 or arg1 */
        if(NEXT_LATCHES.REGS[arg1] < 0){
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
        else if(NEXT_LATCHES.REGS[arg1] == 0){
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        else{
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
        update_latches();
    }
    /* stb instruction */
    else if(strcmp(instruction, "STB") == 0){
        arg2 += arg3;
        MEMORY[arg2][1] = (NEXT_LATCHES.REGS[arg1] & 0x00FF);
        update_latches();
    }
    /* stw instruction */
    else if(strcmp(instruction, "STW") == 0){
        arg2 += arg3;
        MEMORY[arg2][0] = (NEXT_LATCHES.REGS[arg1] & 0x00FF);
        MEMORY[arg2][1] = ((NEXT_LATCHES.REGS[arg1] & 0xFF00) << 8);
        update_latches();
    }
    /* trap instruction */
    else if(strcmp(instruction, "TRAP") == 0){
        NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC;
        NEXT_LATCHES.PC = ((MEMORY[arg1][0]) + (MEMORY[arg1][1] << 8));
        update_latches();
    }
    /* br instruction */
    else if(strcmp(instruction, "BR") == 0){
        if(((line & 0x0E00) == 0x0000) || ((line & 0x0E00) == 0x0E00)){ /* if br or brnzp */
  
	  NEXT_LATCHES.PC = CURRENT_LATCHES.PC + arg1;
        }
        else if((line & 0x0E00) == 0x0800){ /* brn */
            if(CURRENT_LATCHES.N == 1){
                NEXT_LATCHES.PC = CURRENT_LATCHES.PC + arg1;
            }
        }
        else if((line & 0x0E00) == 0x0400){/* brz */
            if(CURRENT_LATCHES.Z == 1){
                NEXT_LATCHES.PC = CURRENT_LATCHES.PC + arg1;
            }
        }
        else if((line & 0x0E00) == 0x0200){/* brp */
            if(CURRENT_LATCHES.P == 1){
                NEXT_LATCHES.PC = CURRENT_LATCHES.PC + arg1;
            }
        }
        else if((line & 0x0E00) == 0x0600){/* brzp */
            if((CURRENT_LATCHES.Z == 1) || (CURRENT_LATCHES.P == 1)){
                NEXT_LATCHES.PC = CURRENT_LATCHES.PC + arg1;
            }
        }
        else if((line & 0x0E00) == 0x0A00){/* brnp */
            if((CURRENT_LATCHES.N == 1) || (CURRENT_LATCHES.P == 1)){
                NEXT_LATCHES.PC = CURRENT_LATCHES.PC + arg1;
            }
        }
        else if((line & 0x0E00) == 0x0C00){/* brnz */
            if((CURRENT_LATCHES.Z == 1) || (CURRENT_LATCHES.N == 1)){
                NEXT_LATCHES.PC = CURRENT_LATCHES.PC + arg1;
            }
        }
	update_latches();
    }
    
}



#define MAX_INST_LENGTH 10

/***************************************************************/
/* This should check which instruction is to be run and which
 values are needed
 This function uses the following variabes:
 
 registers
 CURRENT_LATCHES
 */
/***************************************************************/

void decode(int instruction){
    int sr1 = 0; /* source 1 register */
    int sr2 = 0; /* source 2 register */
    int dr = 0; /* destination register */
    int imm5 = 0; /* immediate 5 value */
    int imm6 = 0; /* immediate 5 value */
    int imm9 = 0; /* immediate 9 value */
    int amount4 = 0; /* variable for shift instructions */
    int trapvector = 0; /* trap vector */
    int first = (instruction & 0xF000); /* this variable is to mask the first four bits, i.e. first & 0xF000 */
	char decoded[MAX_INST_LENGTH];
    /* ADD instruction */
    if(first == 0x1000){
        strcpy(decoded, "ADD");
        dr = (instruction & 0x0E00) >> 9;
        sr1 = (instruction & 0x01C0) >> 6;
        sr1 = NEXT_LATCHES.REGS[sr1];
        if((instruction & 0x0020) == 0x0020){ /* if this add has an immediate 5 value instead of a sr2 value */
            imm5 = (instruction & 0x001F);
            if((imm5 & 0x0010) == 1){
                imm5 = imm5 | 0xFFE0; /* sign extend 1 */
            }
            else{
                imm5 = imm5 & 0x001F; /* sign extend 0 */
            }
            execute(instruction, decoded, dr, sr1, imm5);
        }
        else{
            sr2 = (instruction & 0x0007); /* if the add has a sr2 value instead of a immediate 5 */
            sr2 = NEXT_LATCHES.REGS[sr2];
            execute(instruction, decoded, dr, sr1, sr2);
        }
    }
    /* AND instruction */
    else if(first == 0x5000){
        strcpy(decoded, "AND");
        dr = (instruction & 0x0E00) >> 9;
        sr1 = (instruction & 0x01C0) >> 6;
        sr1 = NEXT_LATCHES.REGS[sr1];
        if((instruction & 0x0020) == 0x0020){
            imm5 = (instruction & 0x001F); /* if the and has a immediate 5 value instead of sr2 */
            if((imm5 & 0x0010) == 1){
                imm5 = imm5 | 0xFFE0; /* sign extend 1 */
            }
            else{
                imm5 = imm5 & 0x001F; /* sign extend 0 */
            }
            execute(instruction, decoded, dr, sr1, imm5);
        }
        else{
            sr2 = (instruction & 0x0007); /* if the and has a sr2 value instead of a immediate 5 */
            sr2 = NEXT_LATCHES.REGS[sr2];
            execute(instruction, decoded, dr, sr1, sr2);
        }
    }
    /* branch instruction or NOP function */
    else if(first == 0x0000){
        if(instruction == 0x0000){
            strcpy(decoded, "NOP");
            execute(instruction, decoded, 0, 0, 0);
        }
        else{
            strcpy(decoded, "BR");
            imm9 = (instruction & 0x01FF);
            if((imm9 & 0x0100) == 0x0100){
                imm9 = imm9 | 0xFE00; /* one extend */
            }
            else{
                imm9 = imm9 & 0x01FF; /* zero extend */
            }
            execute(instruction, decoded, imm9, 0, 0);
        }
    }
    /*ret or jmp function */
    else if(first == 0xC000){
        if((instruction & 0x01C0) == 0x01C0){
            strcpy(decoded, "RET");
            execute(instruction, decoded, 0, 0, 0);
        }
        else{
            strcpy(decoded, "JMP");
            dr = (instruction & 0x01C0) >> 6;
            dr = NEXT_LATCHES.REGS[dr];
            NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC;
            execute(instruction, decoded, dr, 0, 0);
        }
    }
    /* jsr function */
    else if(first == 0x4000){
        if((instruction & 0x0800) == 0x0800){
            strcpy(decoded, "JSR");
            NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC;
            dr = (instruction & 0x07FF); /* this gets the pc offset 11 */
            dr = dr << 1;
            if((dr & 0x0800) == 0){
                dr = dr & 0x0FFF;  /* sign extends 0 */
            }
            else{
                dr = dr | 0xF000;  /* sign extends 1 */
            }
            execute(instruction, decoded, dr, 0, 0);
        }
        else{
            strcpy(decoded, "JSRR");
            NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC;
            dr = (instruction & 0x01C0) >> 6;;
            dr = registers[dr];
            execute(instruction, decoded, dr, 0, 0);
        }
    }
    /* ldb instruction */
    else if(first == 0x2000){
        strcpy(decoded, "LDB");
        dr = (instruction & 0x0E00) >> 9;
        sr1 = (instruction & 0x01C0) >> 6;
        sr1 = NEXT_LATCHES.REGS[sr1]; /* base register */
        imm6 = (instruction & 0x003F); /* 6 bit base offset */
	execute(instruction, decoded, dr, sr1, imm6);
    }
    /* ldw instruction */
    else if(first == 0x6000){
        strcpy(decoded, "LDW");
        dr = (instruction & 0x0E00) >> 9;
        sr1 = (instruction & 0x01C0) >> 6;
        sr1= NEXT_LATCHES.REGS[sr1]; /* base register */
        imm6 = (instruction & 0x003F); /* 6 bit base offset */
	execute(instruction, decoded, dr, sr1, imm6);
    }
    /* lea instruction */
    else if(first == 0xE000){
        strcpy(decoded, "LEA");
        dr = (instruction & 0x0E00) >> 9;
        imm9 = (instruction & 0x01FF);
        execute(instruction, decoded, dr, imm9, 0);
    }
    /* RTI instruction is not implemented
     else if(first == 0x8000){
     strcpy(decoded, "RTI");
     }
     */
    /* xor and not instruction */
    else if(first == 0x9000){
        if((instruction & 0x0038) == 0x0000){
            strcpy(decoded, "XOR");
            dr = (instruction & 0x0E00) >> 9;
            sr1 = (instruction & 0x01C0) >> 6;
            sr1 = NEXT_LATCHES.REGS[sr1];
            sr2 = (instruction & 0x0007);
            sr2 = NEXT_LATCHES.REGS[sr2];
            execute(instruction, decoded, dr, sr1, sr2);
        }
        else if((instruction & 0x003F) == 0x003F){
            strcpy(decoded, "NOT");
            dr = (instruction & 0x0E00) >> 9;
            sr1 = (instruction & 0x01C0) > 6;
            sr1 = NEXT_LATCHES.REGS[sr1];
            execute(instruction, decoded, dr, sr1, 0);
        }
        else{
            /* 5 bit immediate must be sign extended */
            strcpy(decoded, "XOR");
            dr = (instruction & 0x0E00) >> 9;
            sr1 = (instruction & 0x01C0) >> 6;
            sr1 = NEXT_LATCHES.REGS[sr1];
            imm5 = (instruction & 0x001F);
            if((imm5 & 0x0010) == 0x0010){
                imm5 = imm5 | 0xFFE0;
            }
            else{
                imm5 = imm5 & 0x001F;
            }
            execute(instruction, decoded, dr, sr1, imm5);
        }
    }
    /* rshfl, rshfa, lshf instructions */
    else if(first == 0xD000){
        if((instruction & 0x0030) == 0x0010){
            strcpy(decoded,"RSHFL");
            dr = (instruction & 0x0E00) >> 9;
            sr1 = (instruction & 0x01C0) >> 6;
            sr1 = NEXT_LATCHES.REGS[sr1];
            amount4 = (instruction & 0x000F);
            execute(instruction, decoded, dr, sr1, amount4);
        }
        else if((instruction & 0x0030) == 0x0030){
            strcpy(decoded,"RSHFA");
            dr = (instruction & 0x0E00) >> 9;
            sr1 = (instruction & 0x01C0) >> 6;
            sr1 = NEXT_LATCHES.REGS[sr1];
            amount4 = (instruction & 0x000F);
            execute(instruction, decoded, dr, sr1, amount4);
        }
        else{
            strcpy(decoded, "LSHF");
            dr = (instruction & 0x0E00) >> 9;
            sr1 = (instruction & 0x01C0) >> 6;
            sr1 = NEXT_LATCHES.REGS[sr1];
            amount4 =(instruction & 0x000F);
            if((amount4 & 0x0008) == 0x0008){
                amount4 = amount4 | 0xFFF0; /* sign extend 1 */
            }
            else{
                amount4 = amount4 & 0x000F; /*sign extend 0 */
            }
            execute(instruction, decoded, dr, sr1, amount4);
        }
    }
    /* stb instruction */
    else if(first == 0x3000){
        strcpy(decoded, "STB");
        dr = (instruction & 0x0E00) >> 9; /* this is the source register */
        sr1 = (instruction & 0x01C0) >> 6; /* this is the base register */
        sr1 = NEXT_LATCHES.REGS[sr1];
        imm6 = (instruction & 0x003F);
        if((imm6 & 0x0020) == 0x0020){
            imm6 = imm6 | 0xFFC0; /* sign extend 1 */
        }
        else{
            imm6 = imm6 & 0x003F; /* sign extend 0 */
        }
        execute(instruction, decoded, dr, sr1, imm6);
    }
    /* stw instruction */
    else if(first == 0x7000){
        strcpy(decoded, "STW");
        dr = (instruction & 0x0E00) >> 9; /* this is the source register */
        sr1 = (instruction & 0x01C0) >> 6; /* this is the base register */
        sr1 = NEXT_LATCHES.REGS[sr1];
        imm6 = (instruction & 0x003F);
        if((imm6 & 0x0020) == 0x0020){
            imm6 = imm6 | 0xFFC0; /* sign extend 1 */
        }
        else{
            imm6 = imm6 & 0x003F; /* sign extend 0 */
        }
        imm6 = imm6 << 1;
        execute(instruction, decoded, dr, sr1, imm6);
    }
    /* trap instruction */
    else if(first == 0xF000){
        strcpy(decoded, "TRAP");
        trapvector = (instruction & 0x00FF);
        trapvector = trapvector & 0x00FF; /* zero extend */
        trapvector = (trapvector << 1);
        execute(instruction, decoded, trapvector, 0 , 0);
    }
}




/***************************************************************/
/* This should look into MEMORY[x][y] and find the instruction
   This function uses the following variables:
   This function will call decode
   MEMORY
   CURRENT_LATCHES
*/
/***************************************************************/
void fetch_instruction(){
  int counter = (CURRENT_LATCHES.PC >> 1); /* addfress of where the program should be stored, in this case 0x3000/2 */
  NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
  CURRENT_LATCHES.PC = NEXT_LATCHES.PC;
  int lbyte = 0; /* for least significant byte */ 
  int mbyte = 1; /* for most significant byte */
  int instruction = 0;
  instruction = (MEMORY[counter][lbyte] + (MEMORY[counter][mbyte] << 8));
  decode(instruction);
}


void process_instruction(){
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */
  int temp = 0;
  fetch_instruction();
  return;
}


