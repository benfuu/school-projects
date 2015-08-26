// StringADT demonstration file main.cpp
#include <stdio.h>
#include <string.h> // declares the built-in C string library functions, like strcat
#include "String.h" // declares our new improved String functions, like utstrcat
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>

/* the stage 4 tests are supposed to crash, so they're commented out by default
 * When you're ready to test them, uncomment the following two lines, then go and 
 * start edit the stage tests to make sure you crash for each one */
//#define READY_FOR_STAGE_4
//#include "MemHeap.h"

void testStage1(void) {
	char p[12];
	const char* q = "Hello World";
	char* s; 
	char* t; 
	unsigned k;
	
	printf("this test should print Hello World three times\n");

	for (k = 0; k < 12; k += 1) {
		p[k] = q[k];
	}
	s = utstrdup(p);
	printf(s);
	printf("\n");

	q = "you goofed!";
	for (k = 0; k < 12; k += 1) {
		p[k] = q[k];
	}
	printf(s);
	printf("\n");
	
	t = utstrdup(s);
	utstrfree(s);

	printf(t);	
	printf("\n");
	utstrfree(t);
}	


void testStage2(void) {
	char c_str1[20] = "hello";
	char* ut_str1;
	char* ut_str2;

	printf("Starting stage 2 tests\n");
	strcat(c_str1, " world");
	printf("%s\n", c_str1); // nothing exciting, prints "hello world"

	ut_str1 = utstrdup("hello ");
	ut_str1 = utstrrealloc(ut_str1, 20);
	utstrcat(ut_str1, c_str1);
	printf("%s\n", ut_str1); // slightly more exciting, prints "hello hello world"

	utstrcat(ut_str1, " world");
	printf("%s\n", ut_str1); // exciting, should print "hello hello world wo", 'cause there's not enough room for the second world

	ut_str2 = utstrdup("");
	ut_str2 = utstrrealloc(ut_str2, 11);
	utstrcpy(ut_str2, ut_str1 + 6);
	printf("%s\n", ut_str2); // back to "hello world"

	ut_str2 = utstrrealloc(ut_str2, 23);
	utstrcat(ut_str2, " ");
	utstrcat(ut_str2, ut_str1);
	printf("%s\n", ut_str2); // now should be "hello world hello hello"
 
	utstrfree(ut_str1);
	utstrfree(ut_str2);
}

#define BIG 1000000
void testStage3(void) {
	int k;
	char* ut_str1 = utstrdup("");
	ut_str1 = utstrrealloc(ut_str1, BIG); // big, big string

	printf("attempting stage 3 test. This shouldn't take long...\n");
	printf("(no really, it shouldn't take long, if it does, you fail this test)\n");
	fflush(stdout);

	for (k = 0; k < BIG; k += 1) {
		utstrcat(ut_str1, "*");
	}
	if (ut_str1[BIG-1] != '*') {
		printf("stage3 fails for not copying all the characters\n");
	} else if (strlen(ut_str1) != BIG) {
		printf("Hmmm, stage3 has something wrong\n");
	} else {
		printf("grats, stage 3 passed (unless it took a long time to print this message)\n");
	}
	utstrfree(ut_str1);
}

#ifdef READY_FOR_STAGE_4
void testStage4(void) {
	char p[20];
	if (! isSaneHeap()) {
		printf("oh goodness! you've corrupted the heap, naughty naughty\n");
		return;
	}
	if (! isEmptyHeap()) {
		printf("Uh Oh! you have a memory leak somewhere, better find it\n");
		return;
	}
	
	/* if we reach this point, the heap is OK */
	printf("woo HOO! the heap is OK, test 4 looks good so far, now we're going to crash...\n");

	/* each of the following lines should crash the program (an assert should fail) 
	 * try each of them in turn to make sure you're catching the obvious mistakes 
	 * just uncomment the line and see what happens (it should crash) 
	 */
	// printf("crashing with utstrlen\n\n\n"); utstrlen("Hello World");	
	// printf("crashing with utstrcpy\n\n\n"); utstrcpy(p, "Hello World");
	// printf("crashing with utstrcat\n\n\n"); utstrcat(p, "Hello World");
	// printf("crashing with utstrfree\n\n\n"); utstrfree((char *)malloc(20));
	// printf("crashing with utstrrealloc\n\n\n"); utstrrealloc((char *)malloc(20), 40);
}
#endif /* READY_FOR_STAGE_4 */

struct BackwardCompare {
	bool operator()(const char* s1, const char* s2) const {
		return utstrcmp(s1, s2) > 0;
	}
};

void testStage5(void) {
	std::vector<const char*> words;
	std::ifstream word_file("american-english.txt");
	char buff[128];
	printf("Loading the dictionary:....."); fflush(stdout);
	while (!word_file.eof()) {
		word_file.getline(buff, 128);
		words.push_back(utstrdup(buff));
	}
	printf("done\n");

	printf("Sorting the dictionary (backwards)....."); fflush(stdout);
	auto start_time = std::chrono::high_resolution_clock().now();
	std::sort(words.begin(), words.end(), BackwardCompare());
	auto end_time = std::chrono::high_resolution_clock().now();
	printf("done!\n");
	uint32_t time_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
	printf("sorting completed in %d microseconds\n", time_us);

	for (uint32_t k = 0; k < 20; k += 1) {
		printf("%s\n", words[k]);
	}
}

int main(void) {
	testStage1();
	testStage2();
	testStage3();

#ifdef READY_FOR_STAGE_4	
	testStage4();
#endif /* READY_FOR_STAGE_4 */

	testStage5();
	return 0;
}