/*
 * Project5.c 
 *
 * Ben Fu
 * Friday 11am-12pm
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Set.h"

/*
 * Several of the required functions have been written already
 * You may use these functions as-is or you may modify them in any way. 
 * Please NOTE, you are responsible for ensuring that the functions work correctly in 
 * your project solution. Just because the function works correctly in my solution does 
 * NOT mean that the function will work correctly in yours. 
 */

/*
 * Design NOTES:
 *
 * The design provided in this starter kit assumes
 * (1) empty sets will be represented with length == 0 and elements == nullptr (i.e., address 0)
 * (2) amortized doubling is not used, and capacity is ignored/unused. Functions should assume that 
 * the amount of storage available in the elements[] array is equal to length
 */



/* done for you already */
void destroySet(Set* self) {
	free(self->elements);
}

/* done for you already */
void createEmptySet(Set* self) {
	self->len = 0;
	self->elements = 0;
}

/* done for you already */
void createSingletonSet(Set* self, int x) {
	self->elements = (int*) malloc(sizeof(int));
	self->elements[0] = x;
	self->len = 1;
}

/* done for you already */
void createCopySet(Set* self, const Set* other) {
	self->elements = (int*) malloc(other->len * sizeof(int));
	for (int k = 0; k < other->len; k += 1) {
		self->elements[k] = other->elements[k];
	}
	self->len = other->len;	
}

/* done for you already */
void assignSet(Set* self, const Set* other) {
	if (self == other) { return; }
	
	destroySet(self);
	createCopySet(self, other);
}

/* return true if x is an element of self */
bool isMemberSet(const Set* self, int x) {
	if (self->len == 0) { //if array is empty, x is not in set
		return false;
	}

	int lo = 0;					//[lo,hi) represent range of the data in self
	int hi = self->len;

	while (lo < hi - 1) { // repeat while data[lo, hi) contains more than one element
		int m = (lo + hi) / 2;
		if (x < self->elements[m]) { // x cannot be in data[m, n)
			/* if x can be found, it must be in data[lo, m) */
			hi = m;
		}
		else { // x >= data[m] implies x cannot be in data[0, m)
			lo = m;
		}
		/* invariant is restored, x must be in data[lo, hi)
		* and hi - lo is half what it was on the previous iteration */
	}

	/* when the loop termnates we know hi - lo == 1
	* and if x appears in data[] it is in the range data[lo, hi) */
	if (self->elements[lo] == x) {		//if [lo] matches x, then match was found
		return true;
	}
	else {
		return false;
	}
}

/*
 * add x as a new member to this set. 
 * If x is already a member, then self should not be changed
 * Be sure to restore the design invariant property that elemnts[] remains sorted
 * (yes, you can assume it is sorted when the function is called, that's what an invariant is all about)
 */
void insertSet(Set* self, int x) {
	if (isMemberSet(self, x) == true){				//return if x is already in set
		return;
	}
	else{
		//if only 0 or 1 elements in set
		if (self->len == 0){							//if empty set
			self->elements = (int*)malloc(sizeof(int));		//allocate space for one element
			self->elements[0] = x;
			self->len = 1;
			return;
		}
		if (self->len == 1){							//if only one element
			int a = self->elements[0];				//save first element
			destroySet(self);						//free the one element on the heap
			self->elements = (int*)malloc(2 * sizeof(int));	//allocate space for two elements
			if (a < x){								//assign correct order a,x or x,a depending on which is bigger
				self->elements[0] = a;
				self->elements[1] = x;
			}
			else{ //a>x, so x is first, then a
				self->elements[0] = x;
				self->elements[1] = a;
			}
			self->len = 2;
			return;
		}

		//if 2 or more elements in set
		Set temp = { 0, 0};									//create a temporary set
		temp.len = (self->len) + 1;							//make set one bigger than self to hold the extra x
		temp.elements = (int*)malloc((temp.len)*sizeof(int));
		Set* new_set = &temp;								//call it new_set

		if (x < self->elements[0]){				//if x is smaller than the first element
			new_set->elements[0] = x;			//set first element to x
			for (int i = 0; i < self->len; i += 1){	//copy remaining elements into new_set
				new_set->elements[i + 1] = self->elements[i];
			}
		}
		else{ //x is not smaller than the first element
			int same = 1;								//use as flag to see if appending old data or new data
			for (int k = 0; k < self->len; k += 1){
				if (same == 1){
					new_set->elements[k] = self->elements[k];	//copy data to new set
				}
				else{//same==0
					new_set->elements[k + 1] = self->elements[k];	//new data, so copy to an element ahead of the new data
				}

				if (self->elements[k] < x && self->elements[k + 1]>x){	//find place to put x (at location k+1)
					same = 0;									//stop copying old elements of self
					new_set->elements[k + 1] = x;				//insert x into correct location of new set
				}
			}

			if (same == 1){	//if loop finishes and same is still 1, then x is the biggest number
				new_set->elements[new_set->len-1] = x;		//insert x into largest position
			}
		}
		assignSet(self, new_set);					//copy back new set items into old set
		destroySet(new_set);							//destroy new set
	}
}


/*
 * don't forget: it is OK to try to remove an element
 * that is NOT in the set.  
 * If 'x' is not in the set 'self', then
 * removeSet should do nothing (it's not an error)
 * Otherwise, ('x' IS in the set), remove x. Be sure to update self->length
 * It is not necessary (nor recommended) to call malloc -- if removing an element means the 
 * array on the heap is "too big", that's almost certainly OK, and reallocating a smaller array 
 * is almost definitely NOT worth the trouble
 */
void removeSet(Set* self, int x) {
	if (isMemberSet(self, x) == false){				//return if x is not inset
		return;
	}
	else{
		//if only 1 or 2 elements in set
		if (self->len == 0){							//if empty set
			createEmptySet(self);
			return;
		}
		if (self->len == 1){							//if only one element
			destroySet(self);						//free the one element on the heap
			createEmptySet(self);					//make self an empty set
			return;
		}

		//if 2 or more elements in set
		Set temp = { 0, 0 };									//create a temporary set
		self->len -= 1;								//make set one smaller than self

		int same = 1;								//use as flag to see if appending old data or new data
		for (int k = 0; k < self->len; k += 1){
			if (same == 1){
				//do nothing
			}
			else{//same==0
				self->elements[k] = self->elements[k + 1];	//new data, so copy to an element ahead of the new data to new_set
			}

			if (self->elements[k] == x){		//find x (should be at location k)
				same = 0;									//stop copying old elements of self
				self->elements[k] = self->elements[k + 1];		//replace x with next element of self
			}
		}
	}
}

/* done for you already */
void displaySet(const Set* self) {
	int k;
	
	printf("{");

	if (self->len == 0) { 
		printf("}"); 
	}
	else {
		for (k = 0; k < self->len; k += 1) {
			if (k < self->len - 1) {
				printf("%d,", self->elements[k]);
			} else {
				printf("%d}", self->elements[k]);
			}
		}
	}
}

/* return true if self and other have exactly the same elements */
bool isEqualToSet(const Set* self, const Set* other) {
	if (self->len != other->len){			//if lengths are not equal, then sets cannot be equal
		return false;
	}
	for (int i = 0; i < self->len; i += 1){
		if (self->elements[i] == other->elements[i]){
			//do nothing
		}
		else{	//no match
			return false;
		}
	}
	return true;

}

/* return true if every element of self is also an element of other */
bool isSubsetOf(const Set* self, const Set* other) {
	if (self->len > other->len){				//if length of self is greater, all of it cannot fit into other
		return false;
	}
	if (self->len == 0){						//if empty set, then is a subset
		return true;
	}
	if (other->len == 0){					//if other is empty, self cannot be a subset
		return false;
	}
	int match = 0;							//flag to see if whole set matches or not
	int i = 0;								//pointer to element in self
	int j = 0;								//poitner to element in other
	while (i < self->len && j < other->len){
		if (self->elements[i] == other->elements[j]){
			match += 1;						//tally a match
			i += 1;							//increment pointer to next element of self
			j += 1;							//increment pointer to next element of other
		}
		else{	//no match, so just increment pointer to other BUT not to self
			j += 1;
		}
	}
	//if other has reached the end BUT self has not
	if (match == self->len){ //if the number of matches equals the number of elements in self, then all of self was in other
		return true;	
	}
	else{ return false; }
}
	

/* done for you */
bool isEmptySet(const Set* self) {
	return self->len == 0;
}

/* remove all elements from self that are not also elements of other */
void intersectFromSet(Set* self, const Set* other) {
	Set temp = { 0, 0};									//create a temporary set
	temp.len = 0;										
	temp.elements = (int*)malloc((self->len)*sizeof(int)); //make set length equal to either set because intersection can only be as big as either set
	Set* new_set = &temp;								//rename it new_set

	if (self->len == 0){			//if self is empty, do nothing
		createEmptySet(self);
		return;
	}
	else if (other->len == 0){		//if other is empty, intersection is empty
		destroySet(self);		//deallocate self and make it null
		createEmptySet(self);
		return;
	}
	int i = 0;											//pointer to elements of self
	int j = 0;											//pointer to elements of other
	int k = 0;											//poitner to elements of new_set
	while (i < self->len && j < other->len){		//neither pointer has reached the end of the array yet
		if (self->elements[i] == other->elements[j]){	//if match, write to new array
			new_set->elements[k] = self->elements[i];
			new_set->len += 1;							//increment the length of new set
			k += 1;										//increment all three pointers
			i += 1;									
			j += 1;
		}
		else if (self->elements[i] < other->elements[j]){		//if number in self is smaller than number in other, increment the pointer for self
			i += 1;
		}
		else if (self->elements[i] > other->elements[j]){		//if number in self is greater than number in other, increment poitner for other
			j += 1;
		}
	}
	assignSet(self, new_set);					//copy back new set items into old self
	destroySet(new_set);							//destroy new set
}

/* remove all elements from self that are also elements of other */
void subtractFromSet(Set* self, const Set* other) {
	if (self->len == 0){							//if self is empty, return
		createEmptySet(self);
		return;
	}
	else if (other->len == 0){						//if other is empty, just return self
		return;
	}
	Set temp = { 0, 0};								//create a temporary set
	temp.len = 0;
	temp.elements = (int*)malloc((self->len)*sizeof(int)); //make set length equal to self
	Set* new_set = &temp;								//rename it new_set

	int i = 0;											//pointer to elements of self
	int j = 0;											//pointer to elements of other
	int k = 0;											//poitner to elements of new_set
	while (i < self->len && j < other->len){		//neither pointer has reached the end of the array yet
		if (self->elements[i] == other->elements[j]){	//if match, don't write to new set
			i += 1;
			j += 1;
		}
		else if (self->elements[i] < other->elements[j]){		//if number in self is smaller than number in other, increment the pointer for self
			new_set->elements[k] = self->elements[i];			//write unique number in self to new_set
			new_set->len += 1;									//increment length of new_set
			k += 1;												//increment pointers to new_set and self BUT not other
			i += 1;
		}
		else if (self->elements[i] > other->elements[j]){		//if number in self is greater than number in other, increment poitner for other
			j += 1;
		}
	}
	//write remaining values of self into new_set (if j has reached the end of other BUT i has not reached the end of self)
	for (i; i < self->len; i += 1){	
		new_set->elements[k] = self->elements[i];			//write number into new_set
		new_set->len += 1;									//increment length of new_set
		k += 1;												//increment pointers to new_set and self
	}
	assignSet(self, new_set);					//copy back new set items into old self
	destroySet(new_set);							//destroy new set
}

/* add all elements of other to self (obviously, without creating duplicate elements) */
void unionInSet(Set* self, const Set* other) {
	if (self->len == 0 && other->len == 0){
		createEmptySet(self);
		return;
	}

	Set temp = { 0, 0};									//create a temporary set
	temp.len = 0;
	temp.elements = (int*)malloc((self->len + other->len)*sizeof(int));		//make set length equal to SELF + OTHER (biggest value possible)
	Set* new_set = &temp;								//rename it new_set

	int i = 0;											//pointer to elements of self
	int j = 0;											//pointer to elements of other
	int k = 0;											//poitner to elements of new_set

	while (i < self->len && j < other->len){
		if (self->elements[i] == other->elements[j]){	//match, so only write one to new_set
			if (k == 0 || new_set->elements[k - 1] != self->elements[i]){	//if not a duplicate, or just the first element
				new_set->elements[k] = self->elements[i];	//write number to new_set
				new_set->len += 1;
				k += 1;										//increment new_set to write next number
				i += 1;
				j += 1;
			}
			else {		//duplicate, so don't write into new_set and just increment pointers to self and other
				i += 1;
				j += 1;
			}
		}
		else if (self->elements[i] < other->elements[j]){	//number in self is less than number in other, so write number in self to new_set
			new_set->elements[k] = self->elements[i];
			new_set->len += 1;
			k += 1;
			i += 1;
		}
		else if (self->elements[i] > other->elements[j]){	//number in self is greater than number in other, so write number in other to new_set
			new_set->elements[k] = other->elements[j];
			new_set->len += 1;
			k += 1;
			j += 1;
		}
	}
	//if other is already done BUT self isn't
	for (i; i < self->len; i += 1){
		new_set->elements[k] = self->elements[i];			//write number into new_set
		new_set->len += 1;									//increment length of new_set
		k += 1;												//increment pointers to new_set and self
	}
	//if self is already done BUT other isn't
	for (j; j < other->len; j += 1){
		new_set->elements[k] = other->elements[j];			//write number into new_set
		new_set->len += 1;									//increment length of new_set
		k += 1;												//increment pointers to new_set and other
	}
	assignSet(self, new_set);			//copy back new set items into old self
	destroySet(new_set);					//destroy new set
}