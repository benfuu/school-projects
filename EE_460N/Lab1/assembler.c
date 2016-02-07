/*
	Name 1: Ben Fu
	Name 2: Eugene Choe
	UTEID 1: byf69
	UTEID 2: ec32934
*/
/*
	Lab 1 - Assembler.
	Parses an LC3-b assembly file (.asm) to an object file (.obj).
	Run with these commands:
		gcc -ansi -o assemble assembler.c
		./assemble <source_name>.asm <output_name>.obj
*/

/* For Visual Studio */
#define _CRT_SECURE_NO_WARNINGS

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
/* User headers */

/* File reading/writing */
#define INPUT_FILENO 1
#define OUTPUT_FILENO 2

/* Parsing */
#define MAX_LINE_LENGTH 255
#define MAX_CODE_LENGTH_BIN 17
#define MAX_CODE_LENGTH_HEX 7

/* Label restrictions */
#define MAX_LABEL_LENGTH 20
#define MAX_LABELS 255
enum {
	DONE, OK, EMPTY_LINE
};
/* Helpful Structs */

typedef struct {
	char label[MAX_LABEL_LENGTH + 1];
	int address;
} Label;

typedef struct {
	char* pLabel;
	char** pOpcode;
	char** pArg1;
	char** pArg2;
	char** pArg3;
	char** pArg4;
} Instruction;

/* Globals */
Label labelTable[MAX_LABELS];
/* Important addresses */
int orig = NULL;

/* Function declarations */
void fillSymbols(int, char**);
void parse(int, char**);
int toNum(char*);

/* Instruction functions */

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

/* Label functions */
/* Determines if string is a label. Returns 1 if it is a valid label, -1 if not. */
int isValidLabel(char*);
/* Adds a valid label to a label table. Returns 1 if success, -1 if already in table */
int insertLabel(Label, Label*);
/* Determines if label exists in a label table. Returns address of label if exists, -1 otherwise. */
int findLabel(Label, Label*);

/* Helper functions*/
/* Converts strings to numbers. Hex values must be in form "x3000" and decimal values must be in form "#50". */
int toNum(char*);
/* Parses the LC-3b assembly format */
int readAndParse(FILE*, char*, Instruction*);


int main(int argc, char* argv[]) {
	fillSymbols(argc, argv);
	parse(argc, argv);
	printf("Success! Exiting...\n");
	exit(0);
}

/* Pass 1 - Fill the global symbol table */
void fillSymbols(int argc, char* argv[]) {
	printf("Pass 1\n");
	int pc = NULL;	/* keep track of the program counter */
	/* Create variables for parsing */
	FILE* fp = NULL;
	fp = fopen(argv[INPUT_FILENO], "r");
	if (!fp) {
		printf("ERROR: Cannot open file %s\n", argv[1]);
		exit(4);
	}
	char lLine[MAX_LINE_LENGTH + 1];
	Instruction lInst = {
		.pLabel = NULL,
		.pOpcode = NULL,
		.pArg1 = NULL,
		.pArg2 = NULL,
		.pArg3 = NULL,
		.pArg4 = NULL
	};
	int lRet;
	/* Parse until .ORIG is received */
	while (1) {
		lRet = readAndParse(fp, lLine, &lInst);
		if (lRet != DONE && lRet != EMPTY_LINE) {
			if (strlen(lInst.pOpcode) > 0) {	/* Valid opcode found */
				if (strcmp(".orig", lInst.pOpcode) == 0) { /* orig */
					if (orig != NULL) {
						printf("ERROR: Orig already declared. Exiting with error code 3.");
						exit(3);
					}
					orig = toNum(lInst.pArg1);
					if (orig % 2 == 1) {	/* check for odd origin */
						printf("ERROR: Origin value %s is odd. Exiting with error code 3.", lInst.pArg1);
						exit(3);
					}
					if (orig > 0xFFFF) {	/* check if orig is too big */
						printf("ERROR: Orig value %s too big. Exiting with error code 3.", lInst.pArg1);
						exit(3);
					}
					pc = orig;
					printf("Origin is x%x\n", orig);
					break;
				}
				else {	/* First line was NOT orig */
					printf("ERROR: First line was not ORIG. Exiting with error code 4.");
					exit(4);
				}
			}
			
		}
	}
	/* Parse the rest of the program*/
	int foundEnd = 0;	/* 0 for false, 1 for true */
	while (lRet != DONE) {
		lRet = readAndParse(fp, lLine, &lInst);
		if (lRet != DONE && lRet != EMPTY_LINE) {
			if (strcmp(".end", lInst.pOpcode) == 0) { /* found .end, break */
				foundEnd = 1;
			}
			if (strlen(lInst.pLabel) > 0) {	/* if label is parsed */
				if (isValidLabel(lInst.pLabel) == -1) {	/* not a valid label name */
					printf("ERROR: Illegal label name. Exiting with error code 4.");
					exit(4);
				}
				Label lLabel;
				strcpy(lLabel.label, lInst.pLabel);
				lLabel.address = pc;
				if (insertLabel(lLabel, &labelTable) == -1) {	/* insert into the global label table */
					printf("ERROR: Label already exists. Exiting with error code 4.");
					exit(4);
				}
			}
			pc += 1;	/* increment the pc */
		}
	}
	if (foundEnd == 0) {	/* didn't find .end */
		printf("ERROR: .END not found. Exiting with error code 4.");
		exit(4);
	}
	/* Print label table */
	Label* tablePtr = labelTable;
	printf("Label Table:\n");
	printf("Label\tAddress\n");
	while (strlen(tablePtr->label) > 0) {
		printf("%s\t%x\n", tablePtr->label, tablePtr->address);
		tablePtr += 1;
	}
	printf("\n");
	/* Close file since we are done with it */
	fclose(fp);
}

/* Pass 2 - Parse the assembly */
void parse(int argc, char* argv[]) {
	printf("Pass 2\n");
	int pc = orig;	/* keep track of the program counter */
	/* Create variables for parsing */
	FILE* infile = NULL;
	FILE* outfile = NULL;
	infile = fopen(argv[INPUT_FILENO], "r");
	outfile = fopen(argv[OUTPUT_FILENO], "w");
	if (!infile) {
		printf("Error: Cannot open file %s\n", argv[1]);
		exit(4);
	}
	if (!outfile) {
		printf("Error: Cannot open file %s\n", argv[2]);
		exit(4);
	}
	char lLine[MAX_LINE_LENGTH + 1];
	Instruction lInst = {
		.pLabel = NULL,
		.pOpcode = NULL,
		.pArg1 = NULL,
		.pArg2 = NULL,
		.pArg3 = NULL,
		.pArg4 = NULL
	};
	int lRet;
	/* Parse until after orig */
	while (1) {
		lRet = readAndParse(infile, lLine, &lInst);
		if (lRet != DONE && lRet != EMPTY_LINE) {
			if (lInst.pOpcode) {	/* Valid opcode found */
				if (strcmp(".orig", lInst.pOpcode) == 0) { /* orig */
					pc = orig;
					printf("Origin is x%x\n", orig);
					/* Write first line of output (orig address) */
					char origStr[8];
					sprintf(origStr, "0x%04X\n", orig);
					fputs(origStr, outfile);
					break;
				}
				else {	/* First line was NOT orig */
					printf("ERROR: First line was not ORIG. Exiting with error code 4.");
					exit(4);
				}
			}

		}
	}
	/* Parse the program line by line */
	while (lRet != DONE) {
		lRet = readAndParse(infile, lLine, &lInst);
		if (lRet != DONE && lRet != EMPTY_LINE) {
			if (strcmp(".end", lInst.pOpcode) == 0) { /* found .end, break */
				break;
			}
			printf("PC: x%X\n", pc);
			printInst(&lInst);
			/* Write line to file */
			char outStr[MAX_CODE_LENGTH_HEX + 1];
			int res = convertToMachineCode(&lInst, labelTable, pc);	/* convert current line to machine code */
			sprintf(outStr, "0x%04X\n", res);
			fputs(outStr, outfile);
			printf("Assembled code line:\t%s", outStr);

			pc += 1;	/* increment the pc */
		}
	}
	/* Close files since we are done with them */
	fclose(infile);
	fclose(outfile);
}

/* Instruction functions */
void printInst(Instruction* inst) {
	printf("LABEL:\t%s\n", inst->pLabel);
	printf("OPCODE:\t%s\n", inst->pOpcode);
	printf("ARGS:");
	if (inst->pArg1) { printf("\t1. %s\n", inst->pArg1); }
	if (inst->pArg2) { printf("\t2. %s\n", inst->pArg2); }
	if (inst->pArg3) { printf("\t3. %s\n", inst->pArg3); }
	if (inst->pArg4) { printf("\t4. %s\n", inst->pArg4); }
}

int isOpcode(char* str) {
	if (
		strcmp("add", str) == 0 ||
		strcmp("and", str) == 0 ||
		strcmp("br", str) == 0 ||
		strcmp("brn", str) == 0 ||
		strcmp("brz", str) == 0 ||
		strcmp("brp", str) == 0 ||
		strcmp("brnz", str) == 0 ||
		strcmp("brnp", str) == 0 ||
		strcmp("brzp", str) == 0 ||
		strcmp("brnzp", str) == 0 ||
		strcmp("halt", str) == 0 ||
		strcmp("jmp", str) == 0 ||
		strcmp("jsr", str) == 0 ||
		strcmp("jsrr", str) == 0 ||
		strcmp("ldb", str) == 0 ||
		strcmp("ldw", str) == 0 ||
		strcmp("lea", str) == 0 ||
		strcmp("nop", str) == 0 ||
		strcmp("not", str) == 0 ||
		strcmp("ret", str) == 0 ||
		strcmp("lshf", str) == 0 ||
		strcmp("rshfl", str) == 0 ||
		strcmp("rshfa", str) == 0 ||
		strcmp("rti", str) == 0 ||
		strcmp("stb", str) == 0 ||
		strcmp("stw", str) == 0 ||
		strcmp("trap", str) == 0 ||
		strcmp("xor", str) == 0
		) {
		return 1;
	}
	return -1;
}

int getRegister(char* str) {	/* valid registers are R0 - R7 */
	char* strPtr = str;
	if (*strPtr != 'r') {	/* invalid operand */
		return -1;
	}
	strPtr += 1;	/* increment ptr so now the value should be a constant from 0 to 7 */
	int regNum = strtol(strPtr, NULL, 10);
	if (regNum < 0 || regNum > 7) {	/* invalid register number */
		return -1;
	}
	return regNum;
}

int convertTwosComplement(int num, int b) {
	return num += 1 << b;
}

int isConstant(int num, int b) {	/* b is the number of bits available */
	int cmp = 1 << b - 1;	/* cmp = 2^(b-1) */
	if (num > cmp - 1 || num < -cmp) {	/* out of bounds */
		return -1;
	}
	return 1;
}

int isConstantUnsigned(int num, int b) {	/* b is the number of bits available */
	if (num < 0) {
		return -1;
	}
	int cmp = 1 << b; /* cmp = 2^b */
	if (num > cmp - 1) {	/* out of bounds */
		return -1;
	}
}

int convertToMachineCode(Instruction* inst, Label* table, int pc) {
	int res = 0;
	if (strcmp("add", inst->pOpcode) == 0 || strcmp("and", inst->pOpcode) == 0 || strcmp("xor", inst->pOpcode) == 0) { /* 'add', 'and', and 'xor' only differ by the opcode */
		int opcode;
		if (strcmp("add", inst->pOpcode) == 0) { opcode = 0x1; }	/* add */
		else if (strcmp("and", inst->pOpcode) == 0) { opcode = 0x5; }	/* and*/
		else { opcode = 0x9; }	/* xor */
		res += opcode << 12;
		int dr, sr1, sr2, imm;
		/* Check if arg1 and arg2 are valid registers */
		dr = getRegister(inst->pArg1);
		sr1 = getRegister(inst->pArg2);
		if (dr == -1 || sr1 == -1) {	/* first 2 arguments MUST be registers */
			printf("ERROR: Invalid operand. Exiting with error code 4.");
			exit(4);
		}
		/* Append dr and sr1 to res */
		res += dr << 9;
		res += sr1 << 6;
		/* Check if arg3 is a register or a constant */
		char key = inst->pArg3[0];
		if (key == 'r') {	/* is a register */
			sr2 = getRegister(inst->pArg3);
			if (sr2 == -1) { /* invalid register */
				printf("ERROR: Invalid operand. Exiting with error code 4.");
				exit(4);
			}
			res += sr2;
		}
		else if (key == '#' || key == 'x') {	/* is a constant */
			imm = toNum(inst->pArg3);
			if (isConstant(imm, 5) == -1) {
				printf("ERROR: Invalid constant. Exiting with error code 3.");
				exit(3);
			}
			res += 1 << 5;
			if (imm < 0) {	/* convert negative to positive 2's complement */
				imm = convertTwosComplement(imm, 5);
			}
			res += imm;
		}
		else {
			printf("ERROR: Invalid operand. Exiting with error code 4.");
			exit(4);
		}
		/* Check that arg4 is empty */
		if (strlen(inst->pArg4) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (	/* all 8 branch variants */
		strcmp("br", inst->pOpcode) == 0 ||
		strcmp("brn", inst->pOpcode) == 0 ||
		strcmp("brz", inst->pOpcode) == 0 ||
		strcmp("brp", inst->pOpcode) == 0 ||
		strcmp("brnz", inst->pOpcode) == 0 ||
		strcmp("brnp", inst->pOpcode) == 0 ||
		strcmp("brzp", inst->pOpcode) == 0 ||
		strcmp("brnzp", inst->pOpcode) == 0
		) {
		int opcode = 0;
		if (strcmp("br", inst->pOpcode) == 0 || strcmp("brnzp", inst->pOpcode) == 0) {	/* br and brnzp have same machine code output */
			opcode = 0x7;
		}
		else if (strcmp("brn", inst->pOpcode) == 0) { opcode = 0x4; }
		else if (strcmp("brz", inst->pOpcode) == 0) { opcode = 0x2; }
		else if (strcmp("brp", inst->pOpcode) == 0) { opcode = 0x1; }
		else if (strcmp("brnz", inst->pOpcode) == 0) { opcode = 0x6; }
		else if (strcmp("brnp", inst->pOpcode) == 0) { opcode = 0x5; }
		else if (strcmp("brzp", inst->pOpcode) == 0) { opcode = 0x3; }
		else {	/* invalid branch */
			printf("ERROR: Invalid opcode. Exiting with error code 2.");
			exit(2);
		}
		res += opcode << 9;
		/* Check for valid label */
		char* labelName = inst->pArg1;
		if (isValidLabel(labelName) == -1) {	/* invalid label name */
			printf("ERROR: Invalid label name. Exiting with error code 4.");
			exit(4);
		}
		Label label;
		strcpy(label.label, labelName);
		int addr = findLabel(label, table);
		if (addr == -1) {	/* couldn't find label */
			printf("ERROR: Couldn't find label. Exiting with error code 1.");
			exit(1);
		}
		/* Calculate PCOffset and append */
		int diff = addr - (pc + 1);
		if (isConstant(diff, 9) == -1) {	/* too large to fit in 9 bits */
			printf("ERROR: Offset too large. Exiting with error code 4.");
			exit(4);
		}
		if (diff < 0) {	/* convert negative to positive 2's complement */
			diff = convertTwosComplement(diff, 9);
		}
		res += diff;
		/* Check that arg2 is empty  */
		if (strlen(inst->pArg2) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (strcmp("halt", inst->pOpcode) == 0) {
		res = 0xF025;
		/* Check that all args are empty */
		if (strlen(inst->pArg1) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (strcmp("jmp", inst->pOpcode) == 0) {
		int opcode = 0x60;
		res += opcode << 9;
		/* Check that br is valid */
		int br;
		/* Check if arg1 is valid register */
		br = getRegister(inst->pArg1);
		if (br == -1) {	/* first argument MUST be register */
			printf("ERROR: Invalid operand. Exiting with error code 4.");
			exit(4);
		}
		/* Append br to res */
		res += br << 6;
		/* Check that arg2 is empty */
		if (strlen(inst->pArg2) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (strcmp("jsr", inst->pOpcode) == 0) {
		int opcode = 0x4;
		res += opcode << 12;
		res += 1 << 11;	/* constant bit */
						/* Check for valid label */
		char* labelName = inst->pArg1;
		if (isValidLabel(labelName) == -1) {	/* invalid label name */
			printf("ERROR: Invalid label name. Exiting with error code 4.");
			exit(4);
		}
		Label label;
		strcpy(label.label, labelName);
		int addr = findLabel(label, table);
		if (addr == -1) {	/* couldn't find label */
			printf("ERROR: Couldn't find label. Exiting with error code 1.");
			exit(1);
		}
		/* Calculate PCOffset and append */
		int diff = addr - (pc + 1);
		if (isConstant(diff, 11) == -1) {	/* too large to fit in 9 bits */
			printf("ERROR: Offset too large. Exiting with error code 4.");
			exit(4);
		}
		if (diff < 0) {	/* convert negative to positive 2's complement */
			diff = convertTwosComplement(diff, 11);
		}
		res += diff;
		/* Check that arg2 is empty  */
		if (strlen(inst->pArg2) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (strcmp("jsrr", inst->pOpcode) == 0) {
		int opcode = 0x4;
		res += opcode << 12;
		/* Check that br is valid */
		int br;
		/* Check if arg1 is valid register */
		br = getRegister(inst->pArg1);
		if (br == -1) {	/* first argument MUST be register */
			printf("ERROR: Invalid operand. Exiting with error code 4.");
			exit(4);
		}
		/* Append br to res */
		res += br << 6;
		/* Check that arg2 is empty */
		if (strlen(inst->pArg2) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (strcmp("ldb", inst->pOpcode) == 0 || strcmp("ldw", inst->pOpcode) == 0) {
		int opcode;
		if (strcmp("ldb", inst->pOpcode) == 0) { opcode = 0x2; }	/* ldb */
		else { opcode = 0x6; }	/* ldw */
		res += opcode << 12;
		/* Check that dr and sr are both valid */
		int dr, sr, offset;
		/* Check if arg1 and arg2 are valid registers */
		dr = getRegister(inst->pArg1);
		sr = getRegister(inst->pArg2);
		if (dr == -1 || sr == -1) {	/* first 2 arguments MUST be registers */
			printf("ERROR: Invalid operand. Exiting with error code 4.");
			exit(4);
		}
		/* Append dr and sr1 to res */
		res += dr << 9;
		res += sr << 6;
		/* Check that arg3 is a constant */
		char key = inst->pArg3[0];
		if (key == '#' || key == 'x') {	/* is a constant */
			offset = toNum(inst->pArg3);
			if (isConstant(offset, 6) == -1) {
				printf("ERROR: Invalid constant. Exiting with error code 3.");
				exit(3);
			}
			if (offset < 0) {	/* convert negative to positive 2's complement */
				offset = convertTwosComplement(offset, 6);
			}
			res += offset;
		}
		else {
			printf("ERROR: Invalid operand. Exiting with error code 4.");
			exit(4);
		}
		/* Check that arg4 is empty */
		if (strlen(inst->pArg4) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (strcmp("lea", inst->pOpcode) == 0) {
		int opcode = 0xE;
		res += opcode << 12;
		/* Check that dr is valid */
		int dr;
		/* Check if arg1 is valid register */
		dr = getRegister(inst->pArg1);
		if (dr == -1) {	/* first argument MUST be register */
			printf("ERROR: Invalid operand. Exiting with error code 4.");
			exit(4);
		}
		/* Append dr to res */
		res += dr << 9;
		/* Check for valid label */
		char* labelName = inst->pArg2;
		if (isValidLabel(labelName) == -1) {	/* invalid label name */
			printf("ERROR: Invalid label name. Exiting with error code 4.");
			exit(4);
		}
		Label label;
		strcpy(label.label, labelName);
		int addr = findLabel(label, table);
		if (addr == -1) {	/* couldn't find label */
			printf("ERROR: Couldn't find label. Exiting with error code 1.");
			exit(1);
		}
		/* Calculate PCOffset and append */
		int diff = addr - (pc + 1);
		if (isConstant(diff, 9) == -1) {	/* too large to fit in 9 bits */
			printf("ERROR: Offset too large. Exiting with error code 4.");
			exit(4);
		}
		if (diff < 0) {	/* convert negative to positive 2's complement */
			diff = convertTwosComplement(diff, 9);
		}
		res += diff;
		/* Check that arg3 is empty */
		if (strlen(inst->pArg3) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (strcmp("nop", inst->pOpcode) == 0) {
		res = 0x0000;
		/* Check that all args are empty */
		if (strlen(inst->pArg1) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (strcmp("not", inst->pOpcode) == 0) {
		int opcode = 9;
		res += opcode << 12;
		/* Check that dr and sr are both valid */
		int dr, sr;
		/* Check if arg1 and arg2 are valid registers */
		dr = getRegister(inst->pArg1);
		sr = getRegister(inst->pArg2);
		if (dr == -1 || sr == -1) {	/* first 2 arguments MUST be registers */
			printf("ERROR: Invalid operand. Exiting with error code 4.");
			exit(4);
		}
		/* Append dr and sr1 to res */
		res += dr << 9;
		res += sr << 6;
		/* Append trailing 1's */
		res += 0x3F;
		/* Check that arg3 is empty */
		if (strlen(inst->pArg3) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (strcmp("ret", inst->pOpcode) == 0) {
		res = 0xC1C0;
		/* Check that all args are empty */
		if (strlen(inst->pArg1) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (
		strcmp("lshf", inst->pOpcode) == 0 ||
		strcmp("rshfl", inst->pOpcode) == 0 ||
		strcmp("rshfa", inst->pOpcode) == 0
		) {
		int opcode = 0xD;
		res += opcode << 12;
		int dr, sr, num;
		/* Check if arg1 and arg2 are valid registers */
		dr = getRegister(inst->pArg1);
		sr = getRegister(inst->pArg2);
		if (dr == -1 || sr == -1) {	/* first 2 arguments MUST be registers */
			printf("ERROR: Invalid operand. Exiting with error code 4.");
			exit(4);
		}
		/* Append dr and sr1 to res */
		res += dr << 9;
		res += sr << 6;
		/* Check that arg3 is a constant */
		char key = inst->pArg3[0];
		if (key == '#' || key == 'x') {	/* is a constant */
			num = toNum(inst->pArg3);
			if (isConstantUnsigned(num, 4) == -1) {	/* check that it is unsigned */
				printf("ERROR: Invalid constant. Exiting with error code 3.");
				exit(3);
			}
			/* set bits 5 and 4 */
			if (strcmp("rshfa", inst->pOpcode) == 0) {	/* 1 1 */
				res += 0x3 << 4;
			}
			else if (strcmp("rshfl", inst->pOpcode) == 0) {	/* 0 1 */
				res += 0x1 << 4;
			}
			else {}	/* 0 0 */
			res += num;
		}
		else {
			printf("ERROR: Invalid operand. Exiting with error code 4.");
			exit(4);
		}
		/* Check that arg4 is empty */
		if (strlen(inst->pArg4) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (strcmp("rti", inst->pOpcode) == 0) {
		res = 0x8000;
		/* Check that all args are empty */
		if (strlen(inst->pArg1) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (strcmp("stb", inst->pOpcode) == 0 || strcmp("stw", inst->pOpcode) == 0) {
		int opcode;
		if (strcmp("stb", inst->pOpcode) == 0) { opcode = 0x3; }	/* stb */
		else { opcode = 0x7; }	/* stw */
		res += opcode << 12;
		/* Check that dr and sr are both valid */
		int sr, br, offset;
		/* Check if arg1 and arg2 are valid registers */
		sr = getRegister(inst->pArg1);
		br = getRegister(inst->pArg2);
		if (sr == -1 || br == -1) {	/* first 2 arguments MUST be registers */
			printf("ERROR: Invalid operand. Exiting with error code 4.");
			exit(4);
		}
		/* Append dr and sr1 to res */
		res += sr << 9;
		res += br << 6;
		/* Check that arg3 is a constant */
		char key = inst->pArg3[0];
		if (key == '#' || key == 'x') {	/* is a constant */
			offset = toNum(inst->pArg3);
			if (isConstant(offset, 6) == -1) {
				printf("ERROR: Invalid constant. Exiting with error code 3.");
				exit(3);
			}
			if (offset < 0) {	/* convert negative to positive 2's complement */
				offset = convertTwosComplement(offset, 6);
			}
			res += offset;
		}
		else {
			printf("ERROR: Invalid operand. Exiting with error code 4.");
			exit(4);
		}
		/* Check that arg4 is empty */
		if (strlen(inst->pArg4) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (strcmp("trap", inst->pOpcode) == 0) {
		int opcode = 0xF0;
		res += opcode << 8;
		/* Check that arg1 is a hex value */
		char key = inst->pArg1[0];
		int val = 0;
		if (key == '#' || key == 'x') {	/* is an unsigned constant */
			val = toNum(inst->pArg1);
			if (isConstantUnsigned(val, 8) == -1) {
				printf("ERROR: Invalid constant. Exiting with error code 3.");
				exit(3);
			}
		}
		else {	/* not a valid trap code */
			printf("ERROR: Invalid operand. Exiting with error code 4.");
			exit(4);
		}
		res += val;
		/* Check that arg2 is empty */
		if (strlen(inst->pArg2) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else if (strcmp(".fill", inst->pOpcode) == 0) {
		int num = toNum(inst->pArg1);

		if (num < 0) {	/* check for signed overflow */
			if (isConstant(num, 16) == -1) {
				printf("ERROR: Invalid constant. Exiting with error code 3.");
				exit(3);
			}
			num = convertTwosComplement(num, 16);
		}
		else {	/* check for unsigned overflow */
			if (isConstantUnsigned(num, 16) == -1) {
				printf("ERROR: Invalid constant. Exiting with error code 3.");
				exit(3);
			}
		}
		res = num;
		/* Check that arg2 is empty */
		if (strlen(inst->pArg2) > 0) {	/* extra operands */
			printf("ERROR: Extra operands. Exiting with error code 4.");
			exit(4);
		}
	}
	else {	/* not a valid opcode*/
		printf("ERROR: Not a valid opcode. Exiting with error code 2.");
		exit(2);
	}
	return res;
}

/* Label functions. */
int isValidLabel(char* str) {
	/* Verify that it is not an opcode */
	if (isOpcode(str) == 1) { return -1; }
	/* Check for other illegal names */
	if (
		strcmp("in", str) == 0 ||
		strcmp("out", str) == 0 ||
		strcmp("getc", str) == 0 ||
		strcmp("puts", str) == 0
		) {
		return -1;
	}
	/* Check that every character is alphanumeric */
	int i;
	for (i = 0; i < strlen(str); i++) {
		if (isalnum(str[i]) == 0) { /* NOT alphanumeric */
			return -1;
		}
	}
	/* Labels cannot begin with 'x' or a number */
	if (str[0] == 'x' || isdigit(str[0])) {
		return -1;
	}
	/* Labels cannot be registers */
	if (getRegister(str) != -1) {
		return -1;
	}
	/* Check for empty label name */
	if (strlen(str) <= 0) {
		return -1;
	}
	return 1;
}

int insertLabel(Label label, Label* table) {
	/* Find the next empty entry */
	Label* tablePtr = table;
	while (strlen(tablePtr->label) > 0) {
		if (strcmp(label.label, tablePtr->label) == 0) {	/* label already exists */
			return -1;
		}
		tablePtr += 1;
	}
	/* Insert label into the table */
	strcpy(tablePtr->label, label.label);
	tablePtr->address = label.address;
	return 1;
}

int findLabel(Label label, Label* table) {
	Label* tablePtr = table;
	while (strlen(tablePtr->label) > 0) {	/* look through table */
		if (strcmp(label.label, tablePtr->label) == 0) {	/* found a match */
			return tablePtr->address;
		}
		tablePtr += 1;
	}
	return -1;	/* not found */
}

/* Helper functions. Provided graciously by the TAs */
int toNum(char* pStr) {
	char * t_ptr;
	char * orig_pStr;
	int t_length, k;
	int lNum, lNeg = 0;
	long int lNumLong;

	orig_pStr = pStr;
	if (*pStr == '#')				/* decimal */
	{
		pStr++;
		if (*pStr == '-')			/* dec is negative */
		{
			lNeg = 1;
			pStr++;
		}
		t_ptr = pStr;
		t_length = strlen(t_ptr);
		for (k = 0; k < t_length; k++)
		{
			if (!isdigit(*t_ptr))
			{
				printf("Error: invalid decimal operand, %s\n", orig_pStr);
				exit(4);
			}
			t_ptr++;
		}
		lNum = atoi(pStr);
		if (lNeg)
			lNum = -lNum;

		return lNum;
	}
	else if (*pStr == 'x')	/* hex */
	{
		pStr++;
		if (*pStr == '-')			/* hex is negative */
		{
			lNeg = 1;
			pStr++;
		}
		t_ptr = pStr;
		t_length = strlen(t_ptr);
		for (k = 0; k < t_length; k++)
		{
			if (!isxdigit(*t_ptr))
			{
				printf("Error: invalid hex operand, %s\n", orig_pStr);
				exit(4);
			}
			t_ptr++;
		}
		lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
		lNum = (lNumLong > INT_MAX) ? INT_MAX : lNumLong;
		if (lNeg)
			lNum = -lNum;
		return lNum;
	}
	else
	{
		printf("Error: invalid operand, %s\n", orig_pStr);
		exit(4);
		/*This has been changed from error code 3 to error code 4, see clarification 12 */
	}
}

int readAndParse(FILE * pInfile, char * pLine, Instruction* inst) {
	char * lRet, *lPtr;
	int i;
	if (!fgets(pLine, MAX_LINE_LENGTH, pInfile))
		return(DONE);
	for (i = 0; i < strlen(pLine); i++)
		pLine[i] = tolower(pLine[i]);

	/* convert entire line to lowercase */
	inst->pLabel = inst->pOpcode = inst->pArg1 = inst->pArg2 = inst->pArg3 = inst->pArg4 = pLine + strlen(pLine);

	/* ignore the comments */
	lPtr = pLine;

	while (*lPtr != ';' && *lPtr != '\0' &&
		*lPtr != '\n')
		lPtr++;

	*lPtr = '\0';
	if (!(lPtr = strtok(pLine, "\t\n ,")))
		return(EMPTY_LINE);

	if (isOpcode(lPtr) == -1 && lPtr[0] != '.') /* found a label */
	{
		inst->pLabel = lPtr;
		if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);
	}

	inst->pOpcode = lPtr;

	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	inst->pArg1 = lPtr;

	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	inst->pArg2 = lPtr;
	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	inst->pArg3 = lPtr;

	if (!(lPtr = strtok(NULL, "\t\n ,"))) return(OK);

	inst->pArg4 = lPtr;

	return(OK);
}


