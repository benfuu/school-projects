/*
    Name 1: Ben Fu
    UTEID 1: byf69
*/

#define _CRT_SECURE_NO_WARNINGS

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/* Helper functions */
/* Memory */
#define MEMORY_LATENCY 4
int memCycleCt = MEMORY_LATENCY;
int readMemWord(int);
int readMemByte(int);
void writeMemWord(int, int);
void writeMemByte(int, int);
/* Muxes */
void computeMuxOutputs(void);
void computeDRMUX(void);
void computeSR1MUX(void);
void computeADDR1MUX(void);
void computeADDR2MUX(void);
void computeMARMUX(void);
void computePCMUX(void);
/* Other Gates */
int computeALU(void);
int computeSHF(void);
int computeMDR(void);
/* Helpful arithmetic functions */
int zext(int, int);
int sext(int, int);
int wordToInt(int);
int getBit(int, int);
int setCC(int);

typedef struct Bus_Inputs {
	int GATE_MARMUX;
	int GATE_PC;
	int GATE_ALU;
	int GATE_SHF;
	int GATE_MDR;
} Bus_Inputs;

Bus_Inputs BUS_INPUTS;

typedef struct Mux_Outputs {
	int DRMUX;
	int SR1MUX;
	int ADDR1MUX;
	int ADDR2MUX;
	int MARMUX;
	int PCMUX;
} Mux_Outputs;

Mux_Outputs MUX_OUTPUTS;

#define MAX_INSTRUCTION_LENGTH 5
typedef struct Instruction {
	int opcode;
	int arg1;
	int arg2;
	int arg3;
} Instruction;

void decode(Instruction*, int);

/* Initialize the shuffle program */
void initializeShuffle(void) {
	writeMemByte(0x4000, 0xAA);
	writeMemByte(0x4001, 0xBB);
	writeMemByte(0x4002, 0xCC);
	writeMemByte(0x4003, 0xDD);
	writeMemByte(0x4004, 0xD2);
}

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
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {                                                  
    IRD,
    COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX,
    SR1MUX,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
				      (x[J3] << 3) + (x[J2] << 2) +
				      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return(x[DRMUX]); }
int GetSR1MUX(int *x)        { return(x[SR1MUX]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x08000 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  eval_micro_sequencer();   
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
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
/* Purpose   : Simulate the LC-3b until HALTed.                 */
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
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%.4x\n", BUS);
    printf("MDR          : 0x%.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%.4x\n", CURRENT_LATCHES.MAR);
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
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
}

/************************************************************/
/*                                                          */
/* Procedure : init_memory                                  */
/*                                                          */
/* Purpose   : Zero out the memory array                    */
/*                                                          */
/************************************************************/
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

	/* Load the shuffler program */
	/*initializeShuffle();*/
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void initialize(char *ucode_filename, char *program_filename, int num_prog_files) { 
    int i;
    init_control_store(ucode_filename);

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(program_filename);
	while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);

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
    if (argc < 3) {
	printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argv[2], argc - 2);

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

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/

/*
* Evaluate the address of the next state according to the
* micro sequencer logic. Latch the next microinstruction.
*/
void eval_micro_sequencer() {
	int nextState;
	int ird = GetIRD(CURRENT_LATCHES.MICROINSTRUCTION);
	if (ird == 0) {	/* next state is from J bits */
		int j = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
		int cond = GetCOND(CURRENT_LATCHES.MICROINSTRUCTION);
		if (cond == 0) { /* unconditional */ }
		else if (cond == 1) {	/* memory ready */
			if (CURRENT_LATCHES.READY == 1) {	/* if R == 1, assert */
				j = j | 0x2;	/* set bit 1 */
			}
		}
		else if (cond == 2) {	/* branch */
			if (CURRENT_LATCHES.BEN == 1) {
				j = j | 0x4;	/* set bit 2 */
			}
		}
		else {	/* addressing mode */
			if (getBit(CURRENT_LATCHES.IR, 11) == 1) {	/* IR[11] == 1 */
				j = j | 0x1;	/* set bit 0 */
			}
		}
		nextState = j;
	}
	else {	/* next state is [0,0,IR[15:12]] */
		int ir = CURRENT_LATCHES.IR;
		nextState = ((ir >> 12) & 0xF);
	}
	int i;
	for (i = 0; i < CONTROL_STORE_BITS; i++) {
		NEXT_LATCHES.MICROINSTRUCTION[i] = CONTROL_STORE[nextState][i];
	}
	NEXT_LATCHES.STATE_NUMBER = nextState;	/* for debugging */
	NEXT_LATCHES.READY = 0;	/* reset R bit */
}

/*
* This function emulates memory and the WE logic.
* Keep track of which cycle of MEMEN we are dealing with.
* If fourth, we need to latch Ready bit at the end of
* cycle to prepare microsequencer for the fifth cycle.
*/
void cycle_memory() {
	if ((GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 1) && (CURRENT_LATCHES.READY == 0)) { /* count down cycles */
		if (memCycleCt == 1) { /* assert R bit */
			NEXT_LATCHES.READY = 1;
			memCycleCt = MEMORY_LATENCY;
		}
		else {	/* count down 1 */
			memCycleCt -= 1;
		}
	}
}


/*
* Datapath routine emulating operations before driving the bus.
* Evaluate the input of tristate drivers
*        Gate_MARMUX,
*		 Gate_PC,
*		 Gate_ALU,
*		 Gate_SHF,
*		 Gate_MDR.
*/
void eval_bus_drivers() {
	computeMuxOutputs();	/* simulate all muxes and their output values */
	if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		BUS_INPUTS.GATE_MARMUX = MUX_OUTPUTS.MARMUX;
	}
	else if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		BUS_INPUTS.GATE_PC = CURRENT_LATCHES.PC;
	}
	else if (GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		BUS_INPUTS.GATE_ALU = computeALU();
	}
	else if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		BUS_INPUTS.GATE_SHF = computeSHF();
	}
	else if (GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		BUS_INPUTS.GATE_MDR = computeMDR();
	}
}

/*
* Datapath routine for driving the bus from one of the 5 possible
* tristate drivers.
*/
void drive_bus() {
	BUS = 0x0000;	/* reset the bus */

	if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		BUS = Low16bits(BUS_INPUTS.GATE_MARMUX);
	}
	else if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		BUS = Low16bits(BUS_INPUTS.GATE_PC);
	}
	else if (GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		BUS = Low16bits(BUS_INPUTS.GATE_ALU);
	}
	else if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		BUS = Low16bits(BUS_INPUTS.GATE_SHF);
	}
	else if (GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		BUS = Low16bits(BUS_INPUTS.GATE_MDR);
	}
}

/*
* Datapath routine for computing all functions that need to latch
* values in the data path at the end of this cycle.  Some values
* require sourcing the bus; therefore, this routine has to come
* after drive_bus.
*/
void latch_datapath_values() {
	if (GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		NEXT_LATCHES.MAR = BUS;
	}
	if (GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {	/* memory */
			if (GetR_W(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {	/* read */
				if (CURRENT_LATCHES.READY == 1) {
					/* Always load 2 bytes into MDR. Make last bit of MAR a 0 */
					NEXT_LATCHES.MDR = readMemWord(CURRENT_LATCHES.MAR & 0xFFFE);
				}
			}
			else {	/* illegal write */
				printf("Illegal memory operation: LD.MDR == 1 and R.W == 1.");
				exit(2);
			}
		}
		else {	/* BUS */
			if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {	/* byte */
				NEXT_LATCHES.MDR = BUS & 0xFF;	/* SR[7:0] = BUS[7:0] */
			}
			else {	/* word */
				NEXT_LATCHES.MDR = BUS & 0xFFFF;	/* SR = BUS */
			}
		}
	}
	else if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {	/* Write to memory since LD.MDR == 0 and MIO.EN == 1. Implied that R.W == 1 */
		if (GetR_W(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {	/* double check that R.W == 1 */
			if (CURRENT_LATCHES.READY == 1) {
				if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {	/* write byte of data to memory */
					writeMemByte(CURRENT_LATCHES.MAR, CURRENT_LATCHES.MDR & 0xFF);
				}
				else {	/* write word of data to memory */
					if (CURRENT_LATCHES.MAR % 2 == 1) {	/* unaligned word */
						printf("Address 0x%x is unaligned. Exiting...", CURRENT_LATCHES.MAR);
						exit(2);
					}
					writeMemWord(CURRENT_LATCHES.MAR, CURRENT_LATCHES.MDR & 0xFFFF);
				}
			}
		}
		else {
			printf("Illegal memory operation: LD.MDR == 0 and R.W == 0.");
			exit(2);
		}
	}
	if (GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		NEXT_LATCHES.IR = BUS;
	}
	if (GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {	/* BEN <- IR[11] & N + IR[10] & Z + IR[9] & P */
		NEXT_LATCHES.BEN =
			(getBit(CURRENT_LATCHES.IR, 11) & CURRENT_LATCHES.N) +
			(getBit(CURRENT_LATCHES.IR, 10) & CURRENT_LATCHES.Z) +
			(getBit(CURRENT_LATCHES.IR, 9) & CURRENT_LATCHES.P);
	}
	if (GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		NEXT_LATCHES.REGS[MUX_OUTPUTS.DRMUX] = BUS;
	}
	if (GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		if (getBit(BUS, 15) == 1) {	/* set N bit */
			NEXT_LATCHES.N = 1;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 0;
		}
		else if (BUS == 0) {	/* set Z bit */
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 1;
			NEXT_LATCHES.P = 0;
		}
		else {	/* set P bit */
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 1;
		}
	}
	if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
		computePCMUX();		/* compute the new mux output with bus value */
		NEXT_LATCHES.PC = MUX_OUTPUTS.PCMUX;
	}
}

/* Memory Read/Write Functions */
int readMemWord(int addr) {
	return (MEMORY[addr >> 1][1] << 8) + MEMORY[addr >> 1][0];
}

int readMemByte(int addr) {
	return (addr % 2 == 0) ? MEMORY[addr >> 1][0] : MEMORY[addr >> 1][1];
}

void writeMemWord(int addr, int val) {
	int lsByte = val & 0xFF;
	int msByte = (val >> 8) & 0xFF;
	MEMORY[addr >> 1][0] = lsByte;
	MEMORY[addr >> 1][1] = msByte;
}

void writeMemByte(int addr, int val) {
	if (addr % 2 == 0) {	/* even */
		MEMORY[addr >> 1][0] = val & 0xFF;
	}
	else {	/* odd */
		MEMORY[addr >> 1][1] = val & 0xFF;
	}
}


/* Mux Output Functions */
void computeMuxOutputs(void) {
	computeDRMUX();
	computeSR1MUX();
	computeADDR1MUX();
	computeADDR2MUX();
	computeMARMUX();
	computePCMUX();
}

void computeDRMUX(void) {
	int sel = GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION);
	if (sel == 0) {			/* register defined in IR[11:9] */
		MUX_OUTPUTS.DRMUX = (CURRENT_LATCHES.IR >> 9) & 0x7;	/* bits 11-9 */
	}
	else if (sel == 1) {	/* register 7 ([111]) */
		MUX_OUTPUTS.DRMUX = 7;
	}
}

void computeSR1MUX(void) {
	int sel = GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
	if (sel == 0) {			/* register defined in IR[11:9] */
		MUX_OUTPUTS.SR1MUX = (CURRENT_LATCHES.IR >> 9) & 0x7;	/* bits 11-9 */
	}
	else if (sel == 1) {	/* register defined in IR[8:6] */
		MUX_OUTPUTS.SR1MUX = (CURRENT_LATCHES.IR >> 6) & 0x7;	/* bits 8-6 */
	}
}

void computeADDR1MUX(void) {
	int sel = GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
	if (sel == 0) {			/* PC */
		MUX_OUTPUTS.ADDR1MUX = CURRENT_LATCHES.PC;
	}
	else if (sel == 1) {	/* BR = SR1 = REGS[SR1MUX] */
		MUX_OUTPUTS.ADDR1MUX = CURRENT_LATCHES.REGS[MUX_OUTPUTS.SR1MUX];
	}
}

void computeADDR2MUX(void) {
	int sel = GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION);
	if (sel == 0) {			/* select value 0 */
		MUX_OUTPUTS.ADDR2MUX = 0;
	}
	else if (sel == 1) {	/* SEXT(IR[5:0]) */
		MUX_OUTPUTS.ADDR2MUX = sext(CURRENT_LATCHES.IR & 0x3F, 6);
	}
	else if (sel == 2) {	/* SEXT(IR[8:0]) */
		MUX_OUTPUTS.ADDR2MUX = sext(CURRENT_LATCHES.IR & 0x1FF, 9);
	}
	else if (sel == 3) {	/* SEXT(IR[10:0]) */
		MUX_OUTPUTS.ADDR2MUX = sext(CURRENT_LATCHES.IR & 0x7FF, 11);
	}
}

void computeMARMUX(void) {
	int sel = GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION);
	if (sel == 0) {			/* LSHF(ZEXT(IR[7:0]), 1) */
		MUX_OUTPUTS.MARMUX = (CURRENT_LATCHES.IR & 0xFF) << 1;
	}
	else if (sel == 1) {	/* Output of ADDR1 and ADDR2 depending on LSHF1 */
		if (GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {	/* ADDR1MUX + ADDR2MUX */
			MUX_OUTPUTS.MARMUX = MUX_OUTPUTS.ADDR1MUX + MUX_OUTPUTS.ADDR2MUX;
		}
		else if (GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {	/* ADDR1MUX + LSHF(ADDR2MUX, 1) */
			MUX_OUTPUTS.MARMUX = MUX_OUTPUTS.ADDR1MUX + (MUX_OUTPUTS.ADDR2MUX << 1);
		}
	}
}

void computePCMUX(void) {
	int sel = GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION);
	if (sel == 0) {			/* PC + 2 */
		MUX_OUTPUTS.PCMUX = CURRENT_LATCHES.PC + 2;
	}
	else if (sel == 1) {	/* BUS */
		MUX_OUTPUTS.PCMUX = BUS;
	}
	else if (sel == 2) {	/* Output of ADDR1 and ADDR2 depending on LSHF1 */
		if (GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {	/* ADDR1MUX + ADDR2MUX */
			MUX_OUTPUTS.PCMUX = MUX_OUTPUTS.ADDR1MUX + MUX_OUTPUTS.ADDR2MUX;
		}
		else if (GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {	/* ADDR1MUX + LSHF(ADDR2MUX, 1) */
			MUX_OUTPUTS.PCMUX = MUX_OUTPUTS.ADDR1MUX + (MUX_OUTPUTS.ADDR2MUX << 1);
		}
	}
	else {
		printf("Invalid input for PCMUX.\n");
		exit(2);
	}
}


/* Simulate the ALU and return output */
int computeALU(void) {
	/* Operands */
	int A = CURRENT_LATCHES.REGS[MUX_OUTPUTS.SR1MUX];	/* A = SR1 = REGS[SR1MUX] */
	int B;
	/* Simulate SR2MUX */
	int sel = getBit(CURRENT_LATCHES.IR, 5);
	if (sel == 0) {		/* B = SR2 = REGS[IR[2:0]] */
		B = CURRENT_LATCHES.REGS[CURRENT_LATCHES.IR & 0x7];
	}
	else {				/* B = imm5 = sext(IR[4:0]) */
		B = sext(CURRENT_LATCHES.IR & 0x1F, 5);
	}
	/* Calculate Output */
	int res;
	int aluk = GetALUK(CURRENT_LATCHES.MICROINSTRUCTION);
	if (aluk == 0) {		/* ADD */
		res = A + B;
	}
	else if (aluk == 1) {	/* AND */
		res = A & B;
	}
	else if (aluk == 2) {	/* XOR */
		res = A ^ B;
	}
	else if (aluk == 3) {	/* PASSA */
		res = A;
	}
	return res;
}

/* Simulate the SHF and return output */
int computeSHF(void) {
	int sr = CURRENT_LATCHES.REGS[MUX_OUTPUTS.SR1MUX];	/* SR = SR1 = REGS[SR1MUX] */
	int direction = getBit(CURRENT_LATCHES.IR, 4);	/* IR[4] */
	int amount = CURRENT_LATCHES.IR & 0xF;	/* IR[3:0] */
	int res;
	if (direction == 0) {	/* LSHF */
		res = sr << amount;
	}
	else {	/* RSHF */
		int type = getBit(CURRENT_LATCHES.IR, 5);	/* IR[5] */
		if (type == 0) {	/* RSHFL */
			res = sr >> amount;
		}
		else {	/* RSHFA */
			if (getBit(sr, 15) == 1) {	/* negative in 2's complement */
				int i;
				for (i = 0; i < amount; i++) {
					sr >>= 1;		/* shift right by 1 */
					sr += 0x8000;	/* shift in a 1 to the left */
				}
				res = sr;
			}
			else {	/* positive */
				res = sr >> amount;
			}
		}
	}
	return res;
}

/* Calculate input of MDR gate */
int computeMDR(void) {
	int res;
	if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {	/* byte */
		if (CURRENT_LATCHES.MAR % 2 == 0) {	/* keep lsB [15:8] */
			res = sext(CURRENT_LATCHES.MDR & 0xFF, 8);
		}
		else {	/* keep msB [7:0] */
			res = sext((CURRENT_LATCHES.MDR >> 8) & 0xFF, 8);
		}
	}
	else {	/* word */
		if (CURRENT_LATCHES.MAR % 2 == 1) {	/* unaligned word */
			printf("Address 0x%x is unaligned. Exiting...", CURRENT_LATCHES.MAR);
			exit(2);
		}
		res = CURRENT_LATCHES.MDR & 0xFFFF;	/* word */
	}
	return res;
}

/* Decodes an instruction from the IR */
void decode(Instruction* inst, int ir) {
	inst->opcode = (ir >> 12) & 0xF;
	switch (inst->opcode) {
		case 0:
			break;
		case 1:
			inst->arg1 = (ir >> 9) & 0x7;	/* DR */
			inst->arg2 = (ir >> 6) & 0x7;	/* SR */
			if (getBit(ir, 5) == 1) {	/* imm5 */
				inst->arg3 = wordToInt(sext(ir & 0x1F, 5));
			}
			break;
	}
}

/* Sign extends a 2's complement bit sequence from its original width to 16 bits. */
int sext(int seq, int srcWidth) {
	if (getBit(seq, srcWidth - 1) == 1) {	/* msb is negative */
		int i;
		for (i = srcWidth; i < 16; i++) {
			seq += 1 << i;
		}
	}
	return seq;
}

/* Converts a 2's complement word (16 bits) to C integer (32 bits) */
int wordToInt(int val) {
	if (getBit(val, 15) == 1) {	/* negative, sign extend */
		val |= 0xFFFF0000;
	}
	return val;
}

/* Gets a bit from a bit sequence. */
int getBit(int seq, int index) {
	return (seq >> index) & 0x1;
}

