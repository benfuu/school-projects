/*
 * Project1.cpp
 * 
 * Name: Ben Fu
 * EE312 Fall 2014
 * SpellCheck
 */

#include <stdio.h> // provides declarations for printf and putchar
#include <stdint.h> // provides declarations for int32_t uint32_t and the other (new) standard C types

/* All of your code must be in this file. Please no #includes other than standard system headers (ie.., stdio.h, stdint.h)
 *
 * Many students find it helpful to declare global variables (often arrays). You are welcome to use
 * globals if you find them helfpul. Global variables are by no means necessary for this project.
 */

/* You must write this function (spellCheck). Do not change the way the function is declared (i.e., it has
 * exactly two parameters, each parameter is a standard (mundane) C string (see SpellCheck.pdf).
 * You are expected to use reasonable programming style. I *insist* that you indent 
 * reasonably and consistently in your code. I strongly encourage you to avoid big functions
 * (while there are always exceptions, a good rule of thumb is about 15 lines in a function).
 * So, plan on implementing spellCheck by writing two or three other "support functions" that
 * help make the actual spell checking easier for you.
 * There are no explicit restictions on using functions from the C standard library. However,
 * for this project you should avoid using functionality from the C++ standard libary. You will
 * almost certainly find it easiest to just write everything you need from scratch!
 */
char word[200];						//contains a complete word
char *z = word;						//location of the first letter of a word to be written
char *zz;							//location of word in the article to be written

char* wordCheck(char*);
char* writeWord(char*);
char dicCheck(char*);




void spellCheck(char article[], char dictionary[]) {
	char *x = article;
	while (*x != 0){					//not end of article
		x = wordCheck(x);			//check for 2 consecutive letters and update location of next word to check
		if (*x != 0){
			x = writeWord(zz);			//write word into the global word array

			char *y = dictionary;
			if (dicCheck(y) == 0){		//if doesn't match
				printf("%s\n", word);	//print the word on the screen
			}
			int32_t i;
			for (i = 0; i < 200; i++){	//clear array
				word[i] = 0;
			}
			z = word;
		}
	}			
}

char* wordCheck(char *x) {			//checks if is a proper word (2 consecutive letter)
//	char *begin = x;					//save location of x **actually unnecessary, was only used as a precaution
	char lettercount = 0;			//counts consecutive letters
	while (lettercount < 2){			//consecutive letters have not been found yet
		if (*x == 0){
			return x;				//check for end of word
		}
		char a = *x;					//get character into variable a
		//test for 2 consecutive letters
		if (a >= 'A'){				//uppercase letter
			if (a <= 'Z'){
				lettercount++;		//acknowledge a letter was found
			}
			else if (a >= 'a'){			//lowercase letter
				if (a <= 'z')
					lettercount++;		//acknowledge a letter was found
			}
		}
		else { lettercount = 0; }	//not a letter, so reset count

		x++;							//move on to next letter
	}
	zz = x - 2;						//save location of first letter that was found into global variable
	return x;						//save the location of the next place to look
}

char* writeWord(char *x){				//write word into the global array
	char letterflag = 0;				//create flag to indicate end of a word
	while (letterflag != 1){
		//keep writing into the array until character is not a letter anymore (letterflag will equal 1 if not a letter)
//		if (*x == 0){
//			return x;				//check for end of word				unnecessary, the writer will already KNOW if article string has ended
//		}
		if (*x >= 'A'){				//uppercase letter
			if (*x <= 'Z'){
				*z = *x;				//write letter into array
				z++;
				x++;
			}
			else if (*x >= 'a'){			//lowercase letter
				if (*x <= 'z')
					*z = *x;				//write letter into array
					z++;
					x++;
			}
		}
		else{ letterflag = 1; }		//acknowledge end of word
	}
	*z = 0;							//write endstring (dictionary checker will look for this****very important)
	z = word;						//reinitialize z to the beginning of word array
	return x;
}

char dicCheck(char *y){
	char match = 0;						//flag for a match in the dictionary
	char *begin = z;						//save beginning of word
	while (*z != 0){						//not end of word yet
		if (*y == 0){					//check for end of dictionary
			if (match != 0){
				return 1;				//acknowledge that the word matched
			}
			else { return 0; }			//no match from dictionary was found
		}
		if (*z == *y){				//check for exact match
			z++;						//increment both strings
			y++;
			match++;					//acknowledge a letter match
		}
		else if (*z + 0x20 == *y){	/*check for capital to lowercase, letters in the word
									array HAVE to be proper letters so no need to worry about junk ascii*/
			z++;						//increment both strings
			y++;
			match++;					//acknowledge a letter match
		}
		//not capital, so check for lowercase to capital, ***BUT ONLY CHECK if it is a lowercase letter so you don't get junk ascii match
		else if (*z >= 'a'){			//lowercase letter is ensured so you don't accidentally subtract 0x20 from a capital letter
			if (*z <= 'z'){
				if (*z - 0x20 == *y){
					z++;					//increment both strings
					y++;
					match++;				//acknowledge a letter match
				}
				else{
					match = 0;				//no match
					while (*y != '\n'){
						y++;						//increment dictionary to end of word only if no match
					}
					y++;						//increment one more to go from \n to the first letter of the next word
					z = begin;				//go back to beginning of word
				}
			}
		}
		//no match at all
		else{
			match = 0;				//no match
			while (*y != '\n'){
				y++;						//increment dictionary to end of word only if no match
			}
			y++;						//increment one more to go from \n to the first letter of the next word
			z = begin;				//go back to beginning of word
		}
	}
	/*by this point z should point to the beginning of the word array again, and y should point to the "\n" at the end of a word if a match was found,
	or it point to the end of the dictionary ("0") if no match was found*/

	//check if the dictionary term is longer than the word term
	if (*y != '\n'){					//check for end of dictionary term
		match = 0;					//not an exact match if dictionary term is longer than word
		z = begin;					//go back to beginning of word
	}

	if (match != 0){
		return 1;						//acknowledge that the word matched
	}
	else { return 0; }					//no match from dictionary was found
}

