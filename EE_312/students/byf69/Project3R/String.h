#ifndef _String_h
#define _String_h 1

#include <stdint.h>

typedef 
struct String {
	uint32_t check;
	uint32_t capacity;
	uint32_t length;
	char data[0];
} String;

char* utstrdup(const char* src);
uint32_t utstrlen(const char* src);
char* utstrcat(char* s, const char* suffix); // append the suffix to s
char* utstrcpy(char* dst, const char* src); // overwrite dst with src
void utstrfree(char* self);
char* utstrrealloc(char* s, uint32_t new_capacity);

#endif /* _String_h */
