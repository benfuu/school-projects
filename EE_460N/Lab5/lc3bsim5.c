/*
    Name 1: Ben Fu
    UTEID 1: byf69
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
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
void computeSPMUX(void);
void computeVectorMUX(void);
void computeR6MUX(void);
void computeEXCMUX(void);
void computePSRMUX(void);
void computeVAMUX(void);
void computePTBRMUX(void);
void computePTEMUX(void);
void computeVAMODEMUX(void);
void computeVARSMUX(void);
/* Other Gates */
int computeALU(void);
int computeSHF(void);
int computeMDR(void);
int computeGateVector(void);
int computeGatePTE(void);
/* Helpful arithmetic functions */
int zext(int, int);
int sext(int, int);
int wordToInt(int);
int getBit(int, int);
int setCC(int);
int isProtectionException(int);
int isPageFaultException(int);

typedef struct Bus_Inputs {
    int GATE_MARMUX;
    int GATE_PC;
    int GATE_PCDec;
    int GATE_SP;
    int GATE_Vector;
    int GATE_R6;
    int GATE_SHF;
    int GATE_ALU;
    int GATE_MDR;
    int GATE_PSR;
    int GATE_VA;
    int GATE_PTE;
} Bus_Inputs;
Bus_Inputs BUS_INPUTS;

typedef struct Mux_Outputs {
    int DRMUX;
    int SR1MUX;
    int ADDR1MUX;
    int ADDR2MUX;
    int MARMUX;
    int PCMUX;
    int SPMUX;
    int VectorMUX;
    int R6MUX;
    int EXCMUX;
    int PSRMUX;
    int VAMUX;
    int PTBRMUX;
    int PTEMUX;
    int VAMODEMUX;
    int VARSMUX;
} Mux_Outputs;
Mux_Outputs MUX_OUTPUTS;

/* Initial values of SSP and USP */
#define ADDR_SSP 0x3000
#define ADDR_USP 0xFE00
/* Vectors for interrupts and exceptions */
#define VECTOR_INT 0x01
#define VECTOR_EXC_PAGE_FAULT 0x02
#define VECTOR_EXC_UNALIGNED_ACCESS 0x03
#define VECTOR_EXC_PROTECTION 0x04
#define VECTOR_EXC_INVALID_OPCODE 0x05

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
    IRD1, IRD0,
    COND2, COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    LD_SSP,
    LD_USP,
    LD_Vector,
    LD_INT,
    LD_EXC,
    LD_PSR,
    LD_VA,
    LD_PTE,
    LD_VAMODE,
    LD_VARS,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    GATE_PCDec,
    GATE_SP,
    GATE_Vector,
    GATE_R6,
    GATE_PSR,
    GATE_VA,
    GATE_PTE,
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX0,
    SR1MUX1, SR1MUX0,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    SPMUX,
    VectorMUX2, VectorMUX1, VectorMUX0,
    R6MUX,
    EXCMUX,
    PSRMUX,
    VAMUX,
    PTEMUX,
    VAMODEMUX,
    VARSMUX2, VARSMUX1, VARSMUX0,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
    /* MODIFY: you have to add all your new control signals */
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x) { return((x[IRD1] << 1) + x[IRD0]); }
int GetCOND(int *x) { return((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
int GetJ(int *x) {
    return((x[J5] << 5) + (x[J4] << 4) +
        (x[J3] << 3) + (x[J2] << 2) +
        (x[J1] << 1) + x[J0]);
}
int GetLD_MAR(int *x) { return(x[LD_MAR]); }
int GetLD_MDR(int *x) { return(x[LD_MDR]); }
int GetLD_IR(int *x) { return(x[LD_IR]); }
int GetLD_BEN(int *x) { return(x[LD_BEN]); }
int GetLD_REG(int *x) { return(x[LD_REG]); }
int GetLD_CC(int *x) { return(x[LD_CC]); }
int GetLD_PC(int *x) { return(x[LD_PC]); }
int GetLD_SSP(int *x) { return(x[LD_SSP]); }
int GetLD_USP(int *x) { return(x[LD_USP]); }
int GetLD_Vector(int *x) { return(x[LD_Vector]); }
int GetLD_INT(int *x) { return(x[LD_INT]); }
int GetLD_EXC(int *x) { return(x[LD_EXC]); }
int GetLD_PSR(int *x) { return(x[LD_PSR]); }
int GetLD_VA(int *x) { return(x[LD_VA]); }
int GetLD_PTE(int *x) { return(x[LD_PTE]); }
int GetLD_VAMODE(int *x) { return(x[LD_VAMODE]); }
int GetLD_VARS(int *x) { return(x[LD_VARS]); }
int GetGATE_PC(int *x) { return(x[GATE_PC]); }
int GetGATE_MDR(int *x) { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x) { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x) { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x) { return(x[GATE_SHF]); }
int GetGATE_PCDec(int *x) { return(x[GATE_PCDec]); }
int GetGATE_SP(int *x) { return(x[GATE_SP]); }
int GetGATE_Vector(int *x) { return(x[GATE_Vector]); }
int GetGATE_R6(int *x) { return(x[GATE_R6]); }
int GetGATE_PSR(int *x) { return(x[GATE_PSR]); }
int GetGATE_VA(int *x) { return(x[GATE_VA]); }
int GetGATE_PTE(int *x) { return(x[GATE_PTE]); }
int GetPCMUX(int *x) { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x) { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x) { return((x[SR1MUX1] << 1) + x[SR1MUX0]); }
int GetADDR1MUX(int *x) { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x) { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x) { return(x[MARMUX]); }
int GetSPMUX(int *x) { return(x[SPMUX]); }
int GetVectorMUX(int *x) { return((x[VectorMUX2] << 2) + (x[VectorMUX1] << 1) + x[VectorMUX0]); }
int GetR6MUX(int *x) { return(x[R6MUX]); }
int GetEXCMUX(int *x) { return(x[EXCMUX]); }
int GetPSRMUX(int *x) { return(x[PSRMUX]); }
int GetVAMUX(int *x) { return(x[VAMUX]); }
int GetPTEMUX(int *x) { return(x[PTEMUX]); }
int GetVAMODEMUX(int *x) { return(x[VAMODEMUX]); }
int GetVARSMUX(int *x) { return((x[VARSMUX2] << 2) + (x[VARSMUX1] << 1) + x[VARSMUX0]); }
int GetALUK(int *x) { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x) { return(x[MIO_EN]); }
int GetR_W(int *x) { return(x[R_W]); }
int GetDATA_SIZE(int *x) { return(x[DATA_SIZE]); }
int GetLSHF1(int *x) { return(x[LSHF1]); }
/* MODIFY: you can add more Get functions for your new control signals */

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

#define WORDS_IN_MEM    0x2000 /* 32 frames */ 
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

typedef struct System_Latches_Struct {

    int PC,		/* program counter */
        MDR,	/* memory data register */
        MAR,	/* memory address register */
        IR,		/* instruction register */
        N,		/* n condition bit */
        Z,		/* z condition bit */
        P,		/* p condition bit */
        BEN;    /* ben register */

    int READY;	/* ready bit */
                /* The ready bit is also latched as you don’t want the memory system to assert it
                at a bad point in the cycle*/

    int REGS[LC_3b_REGS]; /* register file. */

    int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microinstruction */

    int STATE_NUMBER; /* Current State Number - Provided for debugging */

    /* For lab 4 */
    int INT; /* Interrupt register */
    int EXC; /* Exception register */
    int PSR; /* PSR register */
    int INTV; /* Interrupt vector register */
    int EXCV; /* Exception vector register */
    int Vector; /* Vector register */
    int SSP; /* System stack pointer */
    int USP; /* User stack pointer */
    /* MODIFY: you should add here any other registers you need to implement interrupts and exceptions */

    /* For lab 5 */
    int PTBR;   /* This is initialized when we load the page table */
    int VA;     /* Temporary VA register */
    int PTE;    /* Temporary PTE register */
    int VAMODE; /* Keeps track of R/W during Virtual Address Translation */
    int VARS;   /* return state */
    /* MODIFY: you should add here any other registers you need to implement virtual memory */

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

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

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
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
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    printf("INT          : 0x%0.4x\n", CURRENT_LATCHES.INT);
    printf("EXC          : 0x%0.4x\n", CURRENT_LATCHES.EXC);
    printf("PSR          : 0x%0.4x\n", CURRENT_LATCHES.PSR);
    printf("Vector       : 0x%0.4x\n", CURRENT_LATCHES.Vector);
    printf("SSP          : 0x%0.4x\n", CURRENT_LATCHES.SSP);
    printf("USP          : 0x%0.4x\n", CURRENT_LATCHES.USP);

    printf("PTBR         : 0x%0.4x\n", CURRENT_LATCHES.PTBR);
    printf("VA           : 0x%0.4x\n", CURRENT_LATCHES.VA);
    printf("PTE          : 0x%0.4x\n", CURRENT_LATCHES.PTE);
    printf("VAMODE       : 0x%0.4x\n", CURRENT_LATCHES.VAMODE);
    printf("VARS         : 0x%0.4x\n", CURRENT_LATCHES.VARS);

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
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

    switch (buffer[0]) {
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
    for (i = 0; i < CONTROL_STORE_ROWS; i++) {
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
            CONTROL_STORE[i][j] = (line[index] == '0') ? 0 : 1;
            index++;
        }

        /* Warn about extra bits in line. */
        if (line[index] != '\0')
            printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
                ucode_filename, i);
    }
    printf("\n");
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

    for (i = 0; i < WORDS_IN_MEM; i++) {
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
void load_program(char *program_filename, int is_virtual_base) {
    FILE * prog;
    int ii, word, program_base, pte, virtual_pc;

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

    if (is_virtual_base) {
        if (CURRENT_LATCHES.PTBR == 0) {
            printf("Error: Page table base not loaded %s\n", program_filename);
            exit(-1);
        }

        /* convert virtual_base to physical_base */
        virtual_pc = program_base << 1;
        pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) |
            MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

        printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
        if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK) {
            program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
            printf("physical base of program: %x\n\n", program_base);
            program_base = program_base >> 1;
        }
        else {
            printf("attempting to load a program into an invalid (non-resident) page\n\n");
            exit(-1);
        }
    }
    else {
        /* is page table */
        CURRENT_LATCHES.PTBR = program_base << 1;
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
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0 && is_virtual_base)
        CURRENT_LATCHES.PC = virtual_pc;

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *ucode_filename, char *pagetable_filename, char *program_filename, int num_prog_files) {
    int i;
    init_control_store(ucode_filename);

    init_memory();
    load_program(pagetable_filename, 0);
    for (i = 0; i < num_prog_files; i++) {
        load_program(program_filename, 1);
        while (*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);

/* MODIFY: you can add more initialization code HERE */
    CURRENT_LATCHES.SSP = ADDR_SSP;	/* Initial value of system stack pointer */
    CURRENT_LATCHES.USP = ADDR_USP; /* Initial value of user stack pointer */
    CURRENT_LATCHES.PSR = 0x8002;	/* Initial value of PSR */
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
    if (argc < 4) {
        printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
            argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argv[2], argv[3], argc - 3);

    if ((dumpsim_file = fopen("dumpsim", "w")) == NULL) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated
   with a "MODIFY:" comment.
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
                j |= 0x2;	/* set bit 1 */
            }
        }
        else if (cond == 2) {	/* branch */
            if (CURRENT_LATCHES.BEN == 1) {
                j |= 0x4;	/* set bit 2 */
            }
        }
        else if (cond == 3) {	/* addressing mode */
            if (getBit(CURRENT_LATCHES.IR, 11) == 1) {	/* IR[11] == 1 */
                j |= 0x1;	/* set bit 0 */
            }
        }
        else if (cond == 4) {	/* interrupt */
            if (CURRENT_LATCHES.INT == 1) {
                j |= 0x8;	/* set bit 4 */
            }
        }
        nextState = j;
    }
    else if (ird == 1) {	/* next state is [0,0,IR[15:12]] */
        int ir = CURRENT_LATCHES.IR;
        nextState = ((ir >> 12) & 0xF);
    }
    else if (ird == 2) {  /* next state is VARS */
        int vars = CURRENT_LATCHES.VARS;
        nextState = vars;
    }
    else if (ird == 3) {    /* exception */
        if (CURRENT_LATCHES.EXC == 0) {
            nextState = 32;
        }
        else if (CURRENT_LATCHES.EXC == 1) {
            nextState = 40;
        }
        else if (CURRENT_LATCHES.EXC == 2) {
            nextState = 48;
        }
        else if (CURRENT_LATCHES.EXC == 3) {
            nextState = 56;
        }
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
*        Gate_PCDec,
*        Gate_SP,
*        Gate_Vector,
*        Gate_R6,
*		 Gate_ALU,
*		 Gate_SHF,
*        Gate_MDR,
*		 Gate_PSR,
*        Gate_VA,
*        Gate_PTE.
*/
void eval_bus_drivers() {
    computeMuxOutputs();	/* simulate all muxes and their output values */
    if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS_INPUTS.GATE_MARMUX = MUX_OUTPUTS.MARMUX;
    }
    else if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS_INPUTS.GATE_PC = CURRENT_LATCHES.PC;
    }
    else if (GetGATE_PCDec(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS_INPUTS.GATE_PCDec = CURRENT_LATCHES.PC - 2;
    }
    else if (GetGATE_SP(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS_INPUTS.GATE_SP = MUX_OUTPUTS.SPMUX;
    }
    else if (GetGATE_Vector(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS_INPUTS.GATE_Vector = computeGateVector();
    }
    else if (GetGATE_R6(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS_INPUTS.GATE_R6 = MUX_OUTPUTS.R6MUX;
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
    else if (GetGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS_INPUTS.GATE_PSR = CURRENT_LATCHES.PSR;
    }
    else if (GetGATE_VA(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS_INPUTS.GATE_VA = MUX_OUTPUTS.VAMUX + MUX_OUTPUTS.PTBRMUX;
    }
    else if (GetGATE_PTE(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS_INPUTS.GATE_PTE = computeGatePTE();
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
    else if (GetGATE_PCDec(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS = Low16bits(BUS_INPUTS.GATE_PCDec);
    }
    else if (GetGATE_SP(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS = Low16bits(BUS_INPUTS.GATE_SP);
    }
    else if (GetGATE_Vector(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS = Low16bits(BUS_INPUTS.GATE_Vector);
    }
    else if (GetGATE_R6(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS = Low16bits(BUS_INPUTS.GATE_R6);
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
    else if (GetGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS = Low16bits(BUS_INPUTS.GATE_PSR);
    }
    else if (GetGATE_VA(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS = Low16bits(BUS_INPUTS.GATE_VA);
    }
    else if (GetGATE_PTE(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        BUS = Low16bits(BUS_INPUTS.GATE_PTE);
    }
}

/*
* Datapath routine for computing all functions that need to latch
* values in the data path at the end of this cycle.  Some values
* require sourcing the bus; therefore, this routine has to come
* after drive_bus.
*/
void latch_datapath_values() {
    if (CYCLE_COUNT == 299) { NEXT_LATCHES.INT = 1; }	/* trigger the interrupt */
    if (GetLD_EXC(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {	/* check for exceptions */

    }
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
                NEXT_LATCHES.MDR = BUS & 0xFF;		/* SR[7:0] = BUS[7:0] */
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
                        /* exit(2); */
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
        /* set the PSR */
        int cc = (NEXT_LATCHES.N << 2) + (NEXT_LATCHES.Z << 1) + NEXT_LATCHES.P;
        NEXT_LATCHES.PSR |= cc; /* set any cc bits that are different */
        NEXT_LATCHES.PSR &= cc | (CURRENT_LATCHES.PSR & 0x8000);    /* preserve PSR[15] and clear cc bits that are different */
    }
    if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        computePCMUX();		/* compute new mux output with bus value */
        NEXT_LATCHES.PC = Low16bits(MUX_OUTPUTS.PCMUX);
    }
    if (GetLD_SSP(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        NEXT_LATCHES.SSP = CURRENT_LATCHES.REGS[MUX_OUTPUTS.SR1MUX];
    }
    if (GetLD_USP(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        NEXT_LATCHES.USP = CURRENT_LATCHES.REGS[MUX_OUTPUTS.SR1MUX];
    }
    if (GetLD_Vector(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        NEXT_LATCHES.Vector = MUX_OUTPUTS.VectorMUX;
    }
    if (GetLD_INT(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        NEXT_LATCHES.INT = 0;
    }
    if (GetLD_EXC(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        computeEXCMUX();    /* compute new mux output with bus value */
        NEXT_LATCHES.EXC = MUX_OUTPUTS.EXCMUX;
    }
    if (GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        computePSRMUX();    /* compute new mux output with bus value */
        NEXT_LATCHES.PSR = MUX_OUTPUTS.PSRMUX;
        /* restore the condition codes */
        NEXT_LATCHES.N = (NEXT_LATCHES.PSR >> 2) & 0x1;
        NEXT_LATCHES.Z = (NEXT_LATCHES.PSR >> 1) & 0x1;
        NEXT_LATCHES.P = NEXT_LATCHES.PSR & 0x1;
    }
    if (GetLD_VA(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        NEXT_LATCHES.VA = CURRENT_LATCHES.MAR;
    }
    if (GetLD_PTE(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        computePTEMUX();    /* compute new mux output with bus value */
        NEXT_LATCHES.PTE = MUX_OUTPUTS.PTEMUX;
    }
    if (GetLD_VAMODE(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        NEXT_LATCHES.VAMODE = MUX_OUTPUTS.VAMODEMUX;
    }
    if (GetLD_VARS(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
        NEXT_LATCHES.VARS = MUX_OUTPUTS.VARSMUX;
    }
}


/***************************************************************/
/*                                                             */
/* Procedure : Helper Functions                                */
/*                                                             */
/* Purpose   : Assist the functions above with small tasks     */
/*                                                             */
/***************************************************************/

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

    computeSPMUX();
    computeVectorMUX();
    computeR6MUX();
    /*computeEXCMUX();*/
    computePSRMUX();

    computeVAMUX();
    computePTBRMUX();
    computePTEMUX();
    computeVAMODEMUX();
    computeVARSMUX();
}

void computeDRMUX(void) {
    int sel = GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    if (sel == 0) {			/* register defined in IR[11:9] */
        MUX_OUTPUTS.DRMUX = (CURRENT_LATCHES.IR >> 9) & 0x7;	/* bits 11-9 */
    }
    else if (sel == 1) {	/* register 7 ([111]) */
        MUX_OUTPUTS.DRMUX = 7;
    }
    else if (sel == 2) {    /* register 6 ([110]) */
        MUX_OUTPUTS.DRMUX = 6;
    }
    else {
        printf("Invalid input for DRMUX.\n");
        exit(2);
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
    else if (sel == 2) {    /* register 6 ([110]) */
        MUX_OUTPUTS.SR1MUX = 6;
    }
    else {
        printf("Invalid input for SR1MUX.\n");
        exit(2);
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
        if (GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION) == 0) {	    /* ADDR1MUX + ADDR2MUX */
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

void computeSPMUX(void) {
    int sel = GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    if (sel == 0) {         /* SSP */
        MUX_OUTPUTS.SPMUX = CURRENT_LATCHES.SSP;
    }
    else if (sel == 1) {    /* USP */
        MUX_OUTPUTS.SPMUX = CURRENT_LATCHES.USP;
    }
}

void computeVectorMUX(void) {
    int sel = GetVectorMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    if (sel == 0) {         /* x01 = Interrupt Vector */
        MUX_OUTPUTS.VectorMUX = VECTOR_INT;
    }
    else if (sel == 1) {    /* x02 = Page Fault Exception Vector */
        MUX_OUTPUTS.VectorMUX = VECTOR_EXC_PAGE_FAULT;
    }
    else if (sel == 2) {    /* x03 = Unaligned Access Exception Vector */
        MUX_OUTPUTS.VectorMUX = VECTOR_EXC_UNALIGNED_ACCESS;
    }
    else if (sel == 3) {    /* x04 = Protection Exception Vector */
        MUX_OUTPUTS.VectorMUX = VECTOR_EXC_PROTECTION;
    }
    else if (sel == 4) {    /* x05 = Invalid Opcode Vector */
        MUX_OUTPUTS.VectorMUX = VECTOR_EXC_INVALID_OPCODE;
    }
}

void computeR6MUX(void) {
    int sel = GetR6MUX(CURRENT_LATCHES.MICROINSTRUCTION);
    if (sel == 0) {         /* REGS[SR1MUX] + 2 */
        MUX_OUTPUTS.R6MUX = CURRENT_LATCHES.REGS[MUX_OUTPUTS.SR1MUX] + 2;
    }
    else if (sel == 1) {    /* REGS[SR2MUX] - 2 */
        MUX_OUTPUTS.R6MUX = CURRENT_LATCHES.REGS[MUX_OUTPUTS.SR1MUX] - 2;
    }
}

void computeEXCMUX(void) {
    int sel = GetEXCMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    if (sel == 0) {         /* select value 0 */
        MUX_OUTPUTS.EXCMUX = 0;
    }
    else if (sel == 1) {    /* check for exception */
        if (GetLD_EXC(CURRENT_LATCHES.MICROINSTRUCTION) == 1) {
            /* Exceptions only happen when PSR[15] == 1, or when a user program is being run */
            int instruction = BUS;  /* assume that value from BUS is a proper instruction */
            int opcode = (instruction >> 12) & 0xF; /* instruction[15:12] */
            int base_r = CURRENT_LATCHES.REGS[(instruction >> 6) & 0x7];  /* BaseR = REGS[instruction[8:6]] */
            int offset6 = instruction & 0x3F;       /* Offset6 = instruction[5:0] */
            int offset9 = instruction & 0x1FF;      /* Offset9 = instruction[8:0] */
            int offset11 = instruction & 0x7FF;     /* Offset11 = instruction[10:0] */
            /*
            * Check first for Unaligned Access. Possible opcodes are:
            *  LDW, STW, JMP/RET, JSR(R)
            */
            int exc_unaligned = 0;
            if (opcode == 6) {          /* LDW */
                if (base_r % 2 == 1) { exc_unaligned = 1; }   /* odd */
            }
            else if (opcode == 7) {     /* STW */
                if (base_r % 2 == 1) { exc_unaligned = 1; }   /* odd */
            }
            else if (opcode == 12) {    /* JMP/RET */
                if (base_r % 2 == 1) { exc_unaligned = 1; }   /* odd */
            }
            else if (opcode == 4) {     /* JSR(R) */
                if (getBit(instruction, 11) == 0) { /* JSRR */
                    if (base_r % 2 == 1) { exc_unaligned = 1; }   /* odd */
                }
            }
            /*
            * Check for Protection Exception. Possible opcodes are:
            *  BR, JMP/RET, JSR(R), LDB, LDW, STB, STW
            */
            int exc_protection = 0;

            if (opcode == 0) {          /* BR */
                exc_protection = isProtectionException(CURRENT_LATCHES.PC + Low16bits((sext(offset9, 9) << 1)));
            }
            else if (opcode == 12) {    /* JMP/RET */
                exc_protection = isProtectionException(base_r);
            }
            else if (opcode == 4) {     /* JSR(R) */
                if (getBit(instruction, 11) == 1) { /* JSR */
                    exc_protection = isProtectionException(CURRENT_LATCHES.PC + Low16bits((sext(offset11, 11) << 1)));
                }
                else {  /* JSRR */
                    exc_protection = isProtectionException(base_r);
                }
            }
            else if (opcode == 2) {     /* LDB */
                exc_protection = isProtectionException(base_r + sext(offset6, 6));
            }
            else if (opcode == 6) {     /* LDW */
                exc_protection = isProtectionException(base_r + Low16bits((sext(offset6, 6) << 1)));
            }
            else if (opcode == 3) {     /* STB */
                exc_protection = isProtectionException(base_r + sext(offset6, 6));
            }
            else if (opcode == 7) {     /* STW */
                exc_protection = isProtectionException(base_r + Low16bits((sext(offset6, 6) << 1)));
            }
            /*
            * Check for Page Fault Exception. Possible opcodes are:
            *  BR, JMP/RET, JSR(R), LDB, LDW, STB, STW
            */
            int exc_page = 0;

            if (opcode == 0) {          /* BR */
                exc_page = isPageFaultException(CURRENT_LATCHES.PC + Low16bits((sext(offset9, 9) << 1)));
            }
            else if (opcode == 12) {    /* JMP/RET */
                exc_page = isPageFaultException(base_r);
            }
            else if (opcode == 4) {     /* JSR(R) */
                if (getBit(instruction, 11) == 1) { /* JSR */
                    exc_page = isPageFaultException(CURRENT_LATCHES.PC + Low16bits((sext(offset11, 11) << 1)));
                }
                else {  /* JSRR */
                    exc_page = isPageFaultException(base_r);
                }
            }
            else if (opcode == 2) {     /* LDB */
                exc_page = isPageFaultException(base_r + sext(offset6, 6));
            }
            else if (opcode == 6) {     /* LDW */
                exc_page = isPageFaultException(base_r + Low16bits((sext(offset6, 6) << 1)));
            }
            else if (opcode == 3) {     /* STB */
                exc_page = isPageFaultException(base_r + sext(offset6, 6));
            }
            else if (opcode == 7) {     /* STW */
                exc_page = isPageFaultException(base_r + Low16bits((sext(offset6, 6) << 1)));
            }
            /* Priority (from highest to lowest) is Unaligned Access -> Protection -> Page Fault */
            if (exc_unaligned == 1) {
                MUX_OUTPUTS.EXCMUX = 2;
            }
            else if (exc_protection == 1) {
                MUX_OUTPUTS.EXCMUX = 3;
            }
            else if (exc_page == 1) {
                MUX_OUTPUTS.EXCMUX = 1;
            }
            else { MUX_OUTPUTS.EXCMUX = 0; }
        }
        else { MUX_OUTPUTS.EXCMUX = 0; }    /* not loading EXC, so no need to check for exception */
    }
}

void computePSRMUX(void) {
    int sel = GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    if (sel == 0) {         /* select value with bit 15 cleared ([0,PSR[14:0]]) */
        MUX_OUTPUTS.PSRMUX = CURRENT_LATCHES.PSR & 0x7FFF;  /* PSR[15] = 0 */
    }
    else if (sel == 1) {    /* BUS */
        MUX_OUTPUTS.PSRMUX = BUS;
    }
}

void computeVAMUX(void) {
    int sel = GetVAMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    if (sel == 0) {         /* select value RSHF(VA[15:9], 8) */
        MUX_OUTPUTS.VAMUX = (CURRENT_LATCHES.VA >> 8) & 0xFE;
    }
    else if (sel == 1) {    /* select VA[8:0] */
        MUX_OUTPUTS.VAMUX = CURRENT_LATCHES.VA & 0x1FF;
    }
}

void computePTBRMUX(void) {
    int sel = GetVAMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    if (sel == 0) {         /* select PTBR */
        MUX_OUTPUTS.PTBRMUX = CURRENT_LATCHES.PTBR;
    }
    else if (sel == 1) {    /* select PTE[13:9] (PFN) */
        MUX_OUTPUTS.PTBRMUX = CURRENT_LATCHES.PTE & PTE_PFN_MASK;
    }
}

void computePTEMUX(void) {
    int sel = GetPTEMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    if (sel == 0) {         /* select PTE | 0x1 | (0x2 if VAMODE is write) */
        MUX_OUTPUTS.PTEMUX = computeGatePTE();
    }
    else if (sel == 1) {    /* select value from bus */
        MUX_OUTPUTS.PTEMUX = BUS;
    }
}

void computeVAMODEMUX(void) {
    int sel = GetVAMODEMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    if (sel == 0) {         /* read */
        MUX_OUTPUTS.VAMODEMUX = 0;
    }
    else if (sel == 1) {    /* write */
        MUX_OUTPUTS.VAMODEMUX = 1;
    }
}

void computeVARSMUX(void) {
    int sel = GetVARSMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    if (sel == 0) {         /* 23 */
        MUX_OUTPUTS.VARSMUX = 23;
    }
    else if (sel == 1) {    /* 24 */
        MUX_OUTPUTS.VARSMUX = 24;
    }
    else if (sel == 2) {    /* 25 */
        MUX_OUTPUTS.VARSMUX = 25;
    }
    else if (sel == 3) {    /* 29 */
        MUX_OUTPUTS.VARSMUX = 29;
    }
    else if (sel == 4) {    /* 33 */
        MUX_OUTPUTS.VARSMUX = 33;
    }
}

/* Simulate the logic between VectorMUX and Gate_Vector */
int computeGateVector(void) {
    return 0x0200 + ((CURRENT_LATCHES.Vector << 1) & 0xFF);  /* 0x0200 + zext(LSHF(Vector, 1)) */
}

/* Simulate logic between PTEMUX and Gate_PTE */
int computeGatePTE(void) {
    int masked_pte = CURRENT_LATCHES.PTE | 0x1;     /* set reference bit */
    return CURRENT_LATCHES.VAMODE == 1 ? masked_pte | 0x2 : masked_pte; /* if mode is write, set Modified bit */
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
            /* exit(2); */
        }
        res = CURRENT_LATCHES.MDR & 0xFFFF;	/* word */
    }
    return res;
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

/* Checks if address is accessing between 0x0000 and 0x2FFF and returns 1 if true, 0 if not. */
int isProtectionException(int addr) {
    if (getBit(CURRENT_LATCHES.PSR, 15) == 0) { return 0; }
    return (addr >= 0x0000 && addr <= 0x2FFF) ? 1 : 0;
}

/* Checks if virtual address is invalid */
int isPageFaultException(int addr) {
    int pte = readMemWord((((addr >> 9) & 0x7F) << 1) + CURRENT_LATCHES.PTBR);
    return (pte & PTE_VALID_MASK) == PTE_VALID_MASK ? 0 : 1;
}
