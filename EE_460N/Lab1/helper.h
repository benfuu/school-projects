/* Helper functions that are provided with Lab1 */
#ifndef HELPER_H
#define HELPER_H

#include "instruction.h"
#include <stdio.h>
/* Converts strings to numbers. Hex values must be in form "x3000" and decimal values must be in form "#50". */
int toNum(char*);
/* Parses the LC-3b assembly format */
int readAndParse(FILE*, char*, Instruction*);

#endif
