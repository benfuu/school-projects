#include <stdio.h>
#include "String.h"
#include "Customer.h"
#include "CustomerDB.h"

void readString(String&);
void readNum(int&);

CustomerDB database;

int num_bottles = 0;
int num_rattles = 0;
int num_diapers = 0;


/* clear the inventory and reset the customer database to empty */
void reset(void) {
	database.clear();
	num_bottles = 0;
	num_rattles = 0;
	num_diapers = 0;
}

/*
 * selectInventItem is a convenience function that allows you
 * to obtain a pointer to the inventory record using the item type name
 * word must be "Bottles", "Diapers" or "Rattles"
 * for example the expression *selectInventItem("Bottles") returns the 
 * current global variable for the number of bottls in the inventory
 */
int* selectInventItem(String word) {
	if (word == "Bottles") {
		return &num_bottles;
	} else if (word == "Diapers") {
		return &num_diapers;
	} else if (word == "Rattles") {
		return &num_rattles;
	}
	
	/* NOT REACHED */
	return 0;
}

/*
 * this overload of selectInventItem is similar to selectInventItem above, however
 * this overload takes a Customer as a second argument and selects the data member
 * of that Customer that matches "word". The function returns a pointer to one of the three data
 * members from the specified customer.
 */
int* selectInventItem(String word, Customer& cust) {
	if (word == "Bottles") {
		return &cust.bottles;
	} else if (word == "Diapers") {
		return &cust.diapers;
	} else if (word == "Rattles") {
		return &cust.rattles;
	}
	
	/* NOT REACHED */
	return 0;
}


/*
 * findMax searches through the CustomerDB "database" and returns the Customer
 * who has purchased the most items of the specified type.
 * type must be one of "Bottles", "Rattles" or "Diapers".
 * 
 * Note: if two or more Customers are tied for having purchased the most of that item type
 * then findMax returns the first Customer in the CustomerDB who has purchased that maximal
 * quantity.
 *
 * Note: in the special case (invalid case) where there are zero Customers in the 
 * CustomerDB, fundMax returns a null pointer (0)
 */
Customer* findMax(String type) {
	Customer* result = 0;
	int max = 0;
	for (int k = 0; k < database.size(); k += 1) {
		Customer& cust = database[k];
		int *p = selectInventItem(type, cust); 
		if (*p > max) {
			max = *p;
			result = &cust;
		}
	}
	
	return result;
}

void processPurchase() {
	//read the customer name, type of purchase, and amount purchased
	String name;
	readString(name);
	String type;
	readString(type);
	int amount;
	readNum(amount);

	Customer* ben = &database[name];
	//if customer purchased 0 of everything, delete his record
	if (*selectInventItem("Bottles", *ben) == 0 && *selectInventItem("Diapers", *ben) == 0 && *selectInventItem("Rattles", *ben) == 0 && amount == 0) {
		database.length -= 1;				//decrement number of customers by 1
		return;
	}
	//valid customer or a valid purchase, so process purchase
	if (*selectInventItem(type) < amount){	//if amount purchase exceeded inventory amount, print error message
		printf("Sorry %s, We only have %d %s\n", name.c_str(), *selectInventItem(type), type.c_str());
		if (*selectInventItem("Bottles", *ben) == 0 && *selectInventItem("Diapers", *ben) == 0 && *selectInventItem("Rattles", *ben) == 0){
			database.length -= 1;
			return;
		}
	}
	else{	//amount was purchaseable, 
		*selectInventItem(type) -= amount;
		*selectInventItem(type, *ben) += amount;
	}
}

void processSummarize() {
	printf("There are %d Bottles, %d Diapers and %d Rattles in inventory\n", *selectInventItem("Bottles"), *selectInventItem("Diapers"), *selectInventItem("Rattles"));
	printf("we have had a total of %d different customers\n", database.length);
	Customer* max = findMax("Bottles");
	if (max != 0){
		printf("%s has purchased the most Bottles (%d) \n", max->name.c_str(), max->bottles);
	}
	else{ printf("no one has purchased any Bottles\n"); }

	max = findMax("Diapers");
	if (max != 0){
		printf("%s has purchased the most Diapers (%d) \n", max->name.c_str(), max->diapers);
	}
	else{ printf("no one has purchased any Diapers\n"); }

	max = findMax("Rattles");
	if (max != 0){
		printf("%s has purchased the most Rattles (%d) \n", max->name.c_str(), max->rattles);
	}
	else{ printf("no one has purchased any Rattles\n"); }

}

void processInventory() {
	String type;								//read inventory type
	readString(type);
	int amount;								//read amount of inventory to add
	readNum(amount);
	*selectInventItem(type) += amount;		//add selected amount of inventory to the correct global type
}