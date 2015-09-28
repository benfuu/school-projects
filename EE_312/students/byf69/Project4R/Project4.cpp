/* 
 * EE312 Simple CRM Project
 *
 * Ben Fu
 byf69
 */

#include <stdio.h>
#include <assert.h>
#include "MyString.h"
#include "Invent.h"

//for customer processing
#define MAX_CUSTOMERS 1000
Customer customers[MAX_CUSTOMERS];
int num_customers = 0;
int num_empty_customers = 0;
int* next_free_customer_ptr = (int*)&customers[0];		//to keep track of next free customer slot
int* same_customer_ptr = (int*)&customers[0];			//create pointer that points to specific customer
//inventories
int inven_bottles = 0;		//keep track of store inventory with global variable
int inven_rattles = 0;
int inven_diapers = 0;
//for summary calculations
int max_bottles = 0;
int max_rattles = 0;
int max_diapers = 0;

//my functions
//INVENTORY

//PURCHASE
int customerCheck(String* cust_name){
	int same_customer = 0;							//create a same customer flag
	for (int i = 0; i < num_customers; i += 1){		//check for same customer
		for (int j = 0; j < (*cust_name).len; j += 1){		//check for name match
			if (((customers[i]).name).ptr[j] == (*cust_name).ptr[j]){	//first letter matches
				same_customer = 1;								//set flag to acknowledge same customer
				same_customer_ptr = (int*)&customers[i];			//save pointer of the same customer
			}
			else{
				same_customer = 0;				//names don't match, clear flag
				j = (*cust_name).len;			//gtfo if a letter doesn't match
			}
		}
		if (same_customer == 1){					//gtfo if customer match has been found
			i = num_customers;
		}
	}
	if (same_customer == 1){						//return 1 for same customer, 0 for new customer
		return 1;
	}
	else{ return 0; }
}

void processPurchase(Customer* ben, String type, int amount){
	//assume text format is correct, so only need to check first letter (optimization)
	if (*type.ptr == 'B'){
		if (inven_bottles - amount >= 0){		//only purchase if inventory has enough
			(*ben).bottles += amount;			//increment customer purchase amount
			inven_bottles -= amount;				//decrement inventory
		}
		else{ 
			printf("Sorry ");					//print error message for bottles
			StringPrint(&(*ben).name);
			printf(", we only have %d Bottles\n", inven_bottles);
		}		
	}
	else if (*type.ptr == 'R'){
		if (inven_rattles - amount >= 0){		//only purchase if inventory has enough
			(*ben).rattles += amount;			//increment customer purchase amount
			inven_rattles -= amount;				//decrement inventory
		}
		else{
			printf("Sorry ");					//print error message for rattles
			StringPrint(&(*ben).name);
			printf(", we only have %d Rattles\n", inven_rattles);
		}
	}
	else if (*type.ptr == 'D'){
		if (inven_diapers - amount >= 0){		//only purchase if inventory has enough
			(*ben).diapers += amount;			//increment customer purchase amount
			inven_diapers -= amount;				//decrement inventory
		}
		else{
			printf("Sorry ");					//print error message for diapers
			StringPrint(&(*ben).name);
			printf(", we only have %d Diapers\n", inven_diapers);
		}
	}
}

//SUMMARIZE
void findMax(const char* type){					//type is type of inventory
	int max = 0;										//to store the maximum amount of inventory purchased
	Customer* max_address = 0;						//local customer to store customer with largest purchase
	if (type == "diapers"){
		for (int i = 0; i < num_customers; i += 1){		//iterate through all the customers
			if (customers[i].diapers > max){				//if customer has bigger diaper
				max_address = &customers[i];				//save address of customer with biggest purchase of diapers
				max = customers[i].diapers;
			}
		}
	}
	else if (type == "bottles"){
		for (int i = 0; i < num_customers; i += 1){		//iterate through all the customers
			if (customers[i].bottles > max){				//if customer has bigger bottle
				max_address = &customers[i];				//save address of customer with biggest purchase of bottles
				max = customers[i].bottles;
			}
		}
	}
	else if (type == "rattles"){
		for (int i = 0; i < num_customers; i += 1){		//iterate through all the customers
			if (customers[i].rattles > max){				//if customer has bigger rattle
				max_address = &customers[i];				//save address of customer with biggest purchase of rattles
				max = customers[i].rattles;
			}
		}
	}
	if (max > 0){
		StringPrint(&(*max_address).name);
		printf(" has purchased the most %s (%d)\n", type, max);
	}
	else{
		printf("no one has purchased any %s\n", type);
	}
}


//project functions
/* clear the inventory and reset te customer database to empty */
void reset(void) {
	inven_bottles = 0;								//clear all inventory
	inven_rattles = 0;
	inven_diapers = 0;
	for (int i = 0; i < num_customers; i += 1){
		StringDestroy(&(customers[i].name));			//deallocate memory
		(customers[i].name).ptr = 0;					//reset name pointer and length
		(customers[i].name).len = 0;
		customers[i].bottles = 0;					//reset all purchases
		customers[i].rattles = 0;
		customers[i].diapers = 0;
	}
	num_customers = 0;								//reset number of customers
	num_empty_customers = 0;							//reset empty customers
	next_free_customer_ptr = (int*)&customers[0];	//reset new customer pointer
	same_customer_ptr = (int*)&customers[0];			//reset same customer pointer
}

void processSummarize() {
	printf("There are %d diapers, %d bottles and %d rattles in inventory\n", inven_diapers, inven_bottles, inven_rattles);
	//check for customers with 0 total purchases, and subtract them from the total # of customers to get actual # of customers
	for (int i = 0; i < num_customers; i += 1){		//iterate through all the customers
		if (customers[i].diapers == 0 && customers[i].bottles == 0 && customers[i].rattles == 0){		//if customer has no purchases
			num_empty_customers += 1;				//record inactive customer
		}
	}
	printf("we have had a total of %d different customers\n", num_customers - num_empty_customers);
	//find max purchases and print them
	findMax("diapers");
	findMax("bottles");
	findMax("rattles");
}

void processPurchase() {
	String cust_name;					//get customer name
	readString(&cust_name);

	String purchase_type;				//get type of purchase
	readString(&purchase_type);

	int purchase_number;					//get amount of item purchased
	readNum(&purchase_number);

	//initialize customer type "ben"
	Customer* ben = 0;

	//check for same customer
	int x = customerCheck(&cust_name);	//if x=0, create a new customer. if x=1, use the same customer

	if (x == 0){			//ben is a new customer
		ben = (Customer*)next_free_customer_ptr;			//create a new customer at the next free address
		(*ben).name = StringDup(&cust_name);				//copy name and length into new customer
		((*ben).name).len = cust_name.len;				
		num_customers += 1;								//increment number of customers
		next_free_customer_ptr = (int*)&customers[num_customers];	//update pointer to point at next free location
	}
	else{				//ben is an existing customer
		ben = (Customer*)same_customer_ptr;				//use existing customer address
	}
	
	//process the purchase
	processPurchase(ben, purchase_type, purchase_number);	//process purchase amount
	
	//delete customer if his or her total purchases is 0

	StringDestroy(&cust_name);
	StringDestroy(&purchase_type);
}

void processInventory() {
	String inven_type;					//get type of inventory
	readString(&inven_type);

	int inven_number;					//get number of inventory to add
	readNum(&inven_number);

	//add inventory to global variables
	if (*inven_type.ptr == 'B'){			//bottles
		inven_bottles += inven_number;
	}
	else if (*inven_type.ptr == 'R'){	//rattles
		inven_rattles += inven_number;
	}
	else if (*inven_type.ptr == 'D'){	//diapers
		inven_diapers += inven_number;
	}

	StringDestroy(&inven_type);			//deallocate memory for string
}

