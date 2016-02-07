/* For Visual Studio */
#define _CRT_SECURE_NO_WARNINGS

#include "label.h"
#include "instruction.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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