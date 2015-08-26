#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "Vector.h"
#include "String.h"
#include "Parse.h"
#include "Expr.h"

int main(void){
	printf("----TEST 1----\n");
	set_input("test1.blip");
	processToken();
	printf("----TEST 2----\n");
	set_input("test2.blip");
	processToken();
	printf("----TEST 3----\n");
	set_input("test3.blip");
	processToken();
	printf("----TEST 4----\n");
	set_input("test4.blip");
	processToken();
	printf("----TEST 5----\n");
	set_input("test5.blip");
	processToken();
	printf("----TEST 6----\n");
	set_input("test6.blip");
	processToken();
	printf("----TEST 7----\n");
	set_input("test7.blip");
	processToken();
	printf("----TEST 8----\n");
	set_input("test8.blip");
	processToken();
}