/* Label struct */
#ifndef LABEL_H
#define LABEL_H

/* Label restrictions */
#define MAX_LABEL_LENGTH 20
#define MAX_LABELS 255
typedef struct {
	char label[MAX_LABEL_LENGTH + 1];
	int address;
} Label;

/* Determines if string is a label. Returns 1 if it is a valid label, -1 if not. */
int isValidLabel(char*);

/* Adds a valid label to a label table. Returns 1 if success, -1 if already in table */
int insertLabel(Label, Label*);

/* Determines if label exists in a label table. Returns address of label if exists, -1 otherwise. */
int findLabel(Label, Label*);

#endif
