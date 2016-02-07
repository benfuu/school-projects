/* For Visual Studio */
#define _CRT_SECURE_NO_WARNINGS

#include "instruction.h"
#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printInst(Instruction* inst) {
	printf("LABEL:\t%s\n", inst->pLabel);
	printf("OPCODE:\t%s\n", inst->pOpcode);
	printf("ARGS:\n");
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
		if (strcmp("br", inst->pOpcode) == 0 || strcmp("br", inst->pOpcode) == 0) {	/* br and brnzp have same machine code output */
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
	/* shouldn't need these condition checks
	else if (strcmp(".orig", inst->pOpcode) == 0) {

	}
	else if (strcmp(".end", inst->pOpcode) == 0) {

	}
	*/
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