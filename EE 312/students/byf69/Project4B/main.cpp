// A minimal Win32 console app

#include <stdio.h> // declarations of FILE and printf
#include <stdlib.h> // includes declaration of exit()
#include "MemHeap.h" // our own implementation of malloc/free (can be helpful when tracking down memory leaks)
#include "MyString.h" // String ADT case study from class
#include "Invent.h"

FILE* input_stream;
String inventory;
String purchase;
String summarize;
String sku;
String quit;

void displayTailAndClose() {
	char* rval;
	char buff[BUFSIZ];
	
	rval = fgets(buff, BUFSIZ, input_stream);
	while (rval != 0) {
		printf(buff);
		rval = fgets(buff, BUFSIZ, input_stream);
	}
	fclose(input_stream);
}

void readInput(const char* file_name) {
	String next_command;
	bool all_done = false;
	
	/* open the file */
	input_stream = fopen(file_name, "r");
	if (input_stream == 0) {
		printf("sorry, I could not open %s for reading\n", file_name);
		exit(-1);
	}
	
	/* main loop, read commands from input file */
	do {
		readString(&next_command);
		if (StringIsEqualTo(&next_command, &inventory)) {
			processInventory();
		} else if (StringIsEqualTo(&next_command, &purchase)) {
			processPurchase();
		} else if (StringIsEqualTo(&next_command, &summarize)) {
			processSummarize();
		} else if (StringIsEqualTo(&next_command, &sku)) {
			processSKU();
		} else if (StringIsEqualTo(&next_command, &quit)) {
			all_done = true;
		} else {
			printf("Encountered an error in input file\n");
			printf("The offending line has been ignored\n");
		}
		StringDestroy(&next_command);
	} while (! all_done);
	reset();
	displayTailAndClose();
}

int main(void) {
	/* we make four "constant" strings for the four comamnds */
	inventory = StringCreate("Inventory");
	purchase = StringCreate("Purchase");
	summarize = StringCreate("Summarize");
	sku = StringCreate("SKU");
	quit = StringCreate("Quit");
	
	readInput("test1.txt");
	readInput("test2.txt");
	readInput("test3.txt");
	readInput("test5.txt");

	/* all the test are done, time to clean up, let's deallocate our "constant strings" */
	StringDestroy(&inventory);
	StringDestroy(&purchase);
	StringDestroy(&summarize);
	StringDestroy(&sku);
	StringDestroy(&quit);

	/* check the heap for memory leaks and other errors */
	if (! isSaneHeap()) {
		printf("oh goodness! you've corrupted the heap, naughty naughty\n");
		return -1;
	}
	if (! isEmptyHeap()) {
		printf("Uh Oh! you have a memory leak somewhere, better find it\n");
		return -1;
	}
	
}

void readNum(int* p) {
	fscanf(input_stream, "%d", p);
}

void readString(String* p) {
	char buff[BUFSIZ];
	fscanf(input_stream, "%s", buff);
	*p = StringCreate(buff);
}
