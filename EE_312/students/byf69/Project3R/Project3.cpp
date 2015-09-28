/*
 * Replace this comment with the standard EE312 file header!
 */

#include <assert.h>
#include <string.h>
#include "MemHeap.h"
#include "String.h"

/* use these two macros and the function functions if you want -- not required */
#define SIGNATURE (~0xdeadbeef)
#define STRING(s) (((String*)s)-1)

/* this simple function can be useful when you implement stage 4
 * you are not required to use the function, and you can implement stage 4 without it */
int32_t isOurs(char* s) {
	if (!isHeapVar(s)) { return 0; }
	if (STRING(s)->check == SIGNATURE) { return 1; }
	else { return 0; }
}

/* allocate a utstring on the heap, initialize the string correctly by copying
 * the characters from 'src' and return a pointer to the first character of actual string data */
char* utstrdup(const char* src) {
	uint32_t i = 0;															//determine length of array
	while (src[i] != 0){		//determine length of string
		i += 1;				//length at the end will be i
	}
	String *utstring = (String*)malloc(sizeof(String) + i + 1);		//allocate memory for String and src and 0;
	(*utstring).length = i;											//length is size of string minus 0 at the end
	(*utstring).capacity = i;										//capacity is total bytes minus 1
	(*utstring).check = SIGNATURE;
	for (uint32_t j = 0; j < i + 1; j++){							//copy src to utstring including 0
		(*utstring).data[j] = src[j];
	}
	char* s = (*utstring).data;
	return s;
}

/* the parameter 'utstr' must be a utstring. Find the length of this string by accessing the meta-data
 * and return that length */
uint32_t utstrlen(const char* utstr) {
	uint32_t* t = (uint32_t*)utstr - 3;
	String* newstr = (String*)t;						//copy address of s into new string
	assert((*newstr).check == SIGNATURE);
	uint32_t strlen = (*newstr).length;				//access the metadata to find length
	return strlen;

}

/* s must be a utstring. suffix can be an ordinary string (or a utstring)
 * append the characters from suffix to the string s. Do not allocate any additional storage, and
 * only append as many characters as will actually fit in s. Update the length meta-data for utstring s
 * and then return s */
char* utstrcat(char* s, const char* suffix) {
	uint32_t* t = (uint32_t*)s - 3;
	String* newstr = (String*)t;					//copy address of s into new string
	assert((*newstr).check == SIGNATURE);
	uint32_t s_cap = (*newstr).capacity;			//get capacity of new string to know max number of characters that can be written
	if (*((uint32_t*)suffix - 3) == SIGNATURE){	//if matches check then suffix is a utstring
		uint32_t* u = (uint32_t*)suffix - 3;
		String* newsuffix = (String*)u;		//copy address of suffix to new string
		if ((*newsuffix).length + (*newstr).length <= s_cap){	//if total length of suffix and current string is smaller than or equal to capacity of s, append all of suffix
			for (uint32_t j = 0; j < (*newsuffix).length; j++){
				(*newstr).data[j + (*newstr).length] = (*newsuffix).data[j];
			}
			(*newstr).length += (*newsuffix).length;		//length is just original plus suffix length
			(*newstr).data[(*newstr).length] = 0;		//write the terminating 0 at the end of the array
		}
		else{		//length of suffix is larger than capacity of s, so write ONLY into block between end of current string and end of capacity
			for (uint32_t j = 0; j < s_cap - (*newstr).length; j++){
				(*newstr).data[j + (*newstr).length] = (*newsuffix).data[j];
			}
			(*newstr).length = s_cap;					//length is just capacity since full
			(*newstr).data[(*newstr).length] = 0;		//write terminating 0
		}
	}
	else{									//suffix is a normal C string
		uint32_t i = 0;
		while (suffix[i] != 0){				//determine length of string
			i += 1;							//actual number of letters will be i
		}
		if ((*newstr).length + i <= s_cap){			//if total length of current string and suffix is smaller than  or equal to capacity of s, append all of suffix
			for (uint32_t j = 0; j < i; j++){
				(*newstr).data[j + (*newstr).length] = suffix[j];		//append characters to new ut string
			}
			(*newstr).length += i;									//length is length of suffix plus current length
			(*newstr).data[(*newstr).length] = 0;					//write terminating 0
		}
		else{
			for (uint32_t j = 0; j < s_cap - (*newstr).length; j++){		//length of suffix is bigger than capacity, so only write as much as the capacity
				(*newstr).data[j + (*newstr).length] = suffix[j];		//copy characters to new ut string
			}
			(*newstr).length = s_cap;				//length is the capacity of the String (took up all of the capacity)
			(*newstr).data[s_cap] = 0;				//write terminating 0
		}
	}
	return s;
}

/* 'dst' must be a utstring. 'src' can be an ordinary string (or a utstring)
 * overwrite the characters in dst with the characters from src. Do not overflow the capacity of dst
 * For example, if src has five characters and dst has capacity for 10, then copy all five characters
 * However, if src has ten characters and dst only has capacity for 8, then copy only the first 8
 * characters. Do not allocate any additional storage, do not change capacity. Update the length
 * meta-data for dst and then return dst */
char* utstrcpy(char* dst, const char* src) {
	uint32_t* t = (uint32_t*)dst - 3;
	String* newstr = (String*)t;				//copy address of s into new string
	assert((*newstr).check == SIGNATURE);
	uint32_t s_cap = (*newstr).capacity;		//get capacity of new string to know max number of characters that can be written
	if (*((uint32_t*)src - 3) == SIGNATURE){	//if matches check then suffix is a utstring
		uint32_t* u = (uint32_t*)src - 3;
		String* newsrc = (String*)u;		//copy address of suffix to new string
		if ((*newsrc).length <= s_cap){		//if length of suffix is smaller than or equal to capacity of s, write all of suffix
			for (uint32_t j = 0; j < (*newsrc).length; j++){
				(*newstr).data[j] = (*newsrc).data[j];
			}
			(*newstr).length = (*newsrc).length;		//length is just length of suffix
			(*newstr).data[(*newstr).length] = 0;		//write the terminating 0 at the end of the array
		}
		else{									//length of suffix is larger than capacity of s, so write only as much as the capacity
			for (uint32_t j = 0; j < s_cap; j++){
				(*newstr).data[j] = (*newsrc).data[j];
			}
			(*newstr).length = s_cap;			//length is just capacity since full
			(*newstr).data[(*newstr).length] = 0;		//write terminating 0
		}
	}
	else{								//suffix is a normal C string
		uint32_t i = 0;
		while (src[i] != 0){					//determine length of string
			i += 1;							//actual number of letters will be i
		}
		if (i <= s_cap){						//if length of suffix is smaller than  or equal to capacity of s, write all of suffix
			for (uint32_t j = 0; j < i; j++){
				(*newstr).data[j] = src[j];		//copy characters to new ut string
			}
			(*newstr).length = i;					//length is length of suffix
			(*newstr).data[i] = 0;					//write terminating 0
		}
		else{
			for (uint32_t j = 0; j < s_cap; j++){	//length of suffix is bigger than capacity, so only write as much as the capacity
				(*newstr).data[j] = src[j];		//copy characters to new ut string
			}
			(*newstr).length = s_cap;				//length is the capacity of the String (took up all of the capacity)
			(*newstr).data[s_cap] = 0;				//write terminating 0
		}
	}
	return dst;
}

/* self must be a utstring. deallocate the storage for this string
 * (i.e., locate the start of the chunk and call free to dispose of the chunk, note that the start of
 * the chunk will be 12 bytes before *self) */
void utstrfree(char* self) {
	//crash check
	uint32_t* t = (uint32_t*)self - 3;
	String* newstr = (String*)t;				//copy address of s into new string
	assert((*newstr).check == SIGNATURE);
	assert(*((uint32_t*)self - 3) == SIGNATURE);		//same as above

	free((uint32_t*)self-3);					//deallocate memory since don't need anymore
}

/* s must be a utstring.
 * ensure that s has capacity at least as large as 'new_capacity'
 * if s already has capacity equal to or larger than new_capacity, then return s
 * if s has less capacity than new_capacity, then allocate new storage with sufficient space to store
 * new_capacity characters (plus a terminating zero), copy the current characters from s into this
 * new storage. Update the meta-data to correctly describe new new utstring you've created, deallocate s
 * and then return a pointer to the first character in the newly allocated storage */
char* utstrrealloc(char* s, uint32_t new_capacity) {
	uint32_t* t = (uint32_t*)s-3;
	String* newstr = (String*)t;						//copy address of s into new string
	assert((*newstr).check == SIGNATURE);
	if ((*newstr).capacity < new_capacity){			//capacity of new string is not enough
		String* utstring = (String*)malloc(sizeof(String) + new_capacity + 1);	//allocate memory for length of whole string plus terminating 0
		for (uint32_t i = 0; i < (*newstr).length + 1; i++){		//copy characters from s into new utstring including terminating 0
			(*utstring).data[i] = (*newstr).data[i];
		}
		(*utstring).length = (*newstr).length;
		(*utstring).capacity = new_capacity;
		(*utstring).check = (*newstr).check;
		utstrfree(s);
		return (char*)(*utstring).data;
	}
	else{					//capacity of new string is enough to fit new capacity
		return s;
	}
}



