/* Instruction struct. Each instruction has an opcode and a maximum of 4 arguments. */
#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "label.h"
#include <stdio.h>
typedef struct {
	char* pLabel;
	char** pOpcode;
	char** pArg1;
	char** pArg2;
	char** pArg3;
	char** pArg4;
} Instruction;

/* Print instruction for debugging */
void printInst(Instruction*);

/* Determines if a string is a valid Opcode. Returns 1 if valid Opcode, -1 otherwise. */
int isOpcode(char*);

/* Determines if an argument is a valid register. Valid registers are R0-R7. Returns register number if valid register, -1 otherwise. */
int getRegister(char*);

/* Converts a negative signed number to 2's complement */
int convertTwosComplement(int, int);

/* Determines if an argument is a valid constant. Returns 1 if valid constant, -1 otherwise. */
int isConstant(int, int);

/* Determines if an argument is a valid constant (unsigned). Returns 1 if valid constant, -1 otherwise. */
int isConstantUnsigned(int, int);

/* Converts an instruction to a line in machine code and retunrs the result as an integer. */
int convertToMachineCode(Instruction*, Label*, int);

#endif
