#include <iostream>
#include <cassert>
#include "String.h"
#include "CustomerDB.h"

using namespace std;

const int default_capacity = 1;

Customer::Customer(String name) { // done, please do not edit
	this->bottles = 0;
	this->rattles = 0;
	this->diapers = 0;
	this->name = name;
}

CustomerDB::CustomerDB(void) { // done, please do not edit
	this->capacity = default_capacity;
	this->data = new Customer[this->capacity];
	this->length = 0;
}

int CustomerDB::size(void) {
	return this->length;
}

CustomerDB::~CustomerDB(void) { // done, please do not edit
	delete[] this->data;
}

void CustomerDB::clear(void) { // done, please do not edit
	delete[] this->data;
	this->capacity = default_capacity;
	this->data = new Customer[this->capacity];
	this->length = 0;
}

Customer& CustomerDB::operator[](int k) { // done, please do not edit
	assert(k >= 0 && k < this->length);
	return this->data[k];
}

Customer& CustomerDB::operator[](String name) { // not done, your effort goes here
	for (int i = 0; i < this->length; i += 1){
		if (this->data[i].name == name){		//if found a match, return the appropriate customer
			return this->data[i];
		}
	}
	//no match, so make a new customer
	if (this->length == this->capacity){		//if length is already at max size
		Customer* copy = new Customer[this->capacity];	//allocate a temporary array to copy the old array into
		for (int i = 0; i < this->length; i += 1){	//copy all the customers to the new database
			copy[i] = this->data[i];
		}
		delete[] this->data;					//deallocate
		this->capacity *= 2;					//double capacity
		this->data = new Customer[this->capacity];	//reallocate with new capacity
		for (int i = 0; i < this->length; i += 1){	//copy all of the customers back to the newly allocated database
			this->data[i] = copy[i];
		}
		delete[] copy;						//deallocate the local array of customers
	}
	this->length += 1;							//add a customer
	this->data[length - 1] = Customer{ name };
	return this->data[length - 1];				//return address of the new customer created
}



bool CustomerDB::isMember(String name) { // not done, your effort goes here
	for (int i = 0; i < this->length; i += 1){
		if (this->data[i].name == name){		//search through database and check if name matches
			return true;
		}
	}
	return false;
}


