#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "Vector.h"
#include "String.h"
#include "Parse.h"
#include "Expr.h"

Vector<Variable> VariableList{};					//symbol table
Variable* isVariable(Variable var){	//return true if variable exists, false if not
	if (VariableList.size() == 0){ return 0; }
	uint32_t i = 0;
	while (i < VariableList.size()){
		Variable temp = VariableList[i];
		if (var.name == temp.name){ return &VariableList[i]; }
		i += 1;
	}
	return false;
}
void reassignVariable(Variable& var){
	uint32_t i = 0;
	while (i < VariableList.size()){
		Variable* temp = &VariableList[i];
		if (var.name == temp->name){	//found variable!
			temp->name = var.name;	//edit the values
			temp->value = var.value;
			return;
		}
		i += 1;
	}
}
void clearVariables(void){
	for (uint32_t i = 0; i < VariableList.size(); i += 1){
		VariableList.pop_back();
	}
}
void printTree(Expr* node){
	if (!node){ return; }
	printTree(node->left);
	printTree(node->right);
	printf("%s", node->name.c_str());
	printf("%d", node->value);
}
void buildTree(Expr* expr, ExprType type){
	read_next_token();								//read next input	
	if (next_token_type == NUMBER){					//set value to the number, don't build any more nodes
		expr->insert(token_number_value);
		return;
	}
	else if (next_token_type == STRING || type == STRINGEXPR){				//copy string, don't build any more nodes
		expr->insert(next_token());
		return;
	}
	else if (next_token_type == SYMBOL){
		if (String{ next_token() } == String{ "!" } ||
			String{ next_token() } == String{ "~" }){			//unary operators, only build left side
			expr->insert(UNARY, next_token());
			buildTree(expr->left, NUMEXPR);
		}
		else if (String{ next_token() } == String{ "+" } ||
			String{ next_token() } == String{ "-" } ||
			String{ next_token() } == String{ "*" } ||
			String{ next_token() } == String{ "/" } ||
			String{ next_token() } == String{ "%" } ||
			String{ next_token() } == String{ "&&" } ||
			String{ next_token() } == String{ "||" } ||
			String{ next_token() } == String{ "<" } ||
			String{ next_token() } == String{ ">" } ||
			String{ next_token() } == String{ "==" } ||
			String{ next_token() } == String{ "!=" } ||
			String{ next_token() } == String{ "<=" } ||
			String{ next_token() } == String{ ">=" }){
			//binary operators, build both left and right side
			expr->insert(BINARY, next_token());
			buildTree(expr->left, NUMEXPR);
			buildTree(expr->right, NUMEXPR);
		}
		else if (String{ next_token() } == String{ "//" }){	//comment
			skip_line();
			return;
		}
		else{	//it's just weird-ass text.
			expr->insert(next_token());
			return;
		}
	}
	else if (next_token_type == NAME){	//text commands
		if (String{ next_token() } == String{ "text" }){
			expr->insert(TEXTEXPR, next_token());
			buildTree(expr->left, STRINGEXPR);
		}
		else if (String{ next_token() } == String{ "output" }){
			expr->insert(TEXTEXPR, next_token());
			buildTree(expr->left, NUMEXPR);
		}
		else if (String{ next_token() } == String{ "var" } ||
			String{ next_token() } == String{ "set" }){
			//variable commands are just binary =, variable goes on left node and value goes on right node
			expr->insert(BINARY, next_token());
			buildTree(expr->left, VARIABLE);		//SPECIAL FLAG for variable call
			buildTree(expr->right, NUMEXPR);
		}
		else{	//just normal text, either a string or variable
			expr->insert(next_token());			//just insert a normal string
			if (type == VARIABLE){				//it's a variable, so change the type
				expr->type = VARIABLE;
			}
			if (type == NUMEXPR){				//called by output, COULD want a variable value so check
				if (isVariable(Variable{ expr->name, 0 })){	//if returns a value, then is a variable
					expr->type = VARIABLE;
				}
			}
			return;
		}
	}
}
void* processTree(Expr* expr, ExprType type){
	if (!expr || expr->type == NULLEXPR){ return 0; }	//done
	if (expr->type == NUMEXPR){
		return &expr->value;
	}
	else if (expr->type == STRINGEXPR){
		return &expr->name;
	}
	else if (expr->type == VARIABLE){
		if (type == NUMEXPR){	//called by output, so return the numerical value of the variable
			Variable* var = isVariable(Variable{ expr->name, 0 });
			if (var){	//variable already exists
				expr->value = var->value;
				return &expr->value;
			}
			else{ return 0; }
		}
		else if (type == TEXTEXPR){
			return &expr->name;
		}
	}
	else if (expr->type == TEXTEXPR){		//print statements
		if (expr->name == String{ "text" }){		//just print whatever is on the left node
			String* s = (String*)processTree(expr->left, NULLEXPR);
			printf("%s", s->c_str());
			//printf("\n");
		}
		else if (expr->name == String{ "output" }){	//evaluate expression on left
			int32_t* a = (int32_t*)processTree(expr->left, NUMEXPR);
			printf("%d", *a);
			//printf("\n");
		}
	}
	else if (expr->type == BINARY){
		if (expr->name == String{ "var" }){		//initialize a variable
			//get all possible values (variables on both nodes)
			String* name1 = (String*)processTree(expr->left, TEXTEXPR);
			int32_t* val1 = (int32_t*)processTree(expr->left, NUMEXPR);
			if (expr->right->type == VARIABLE){		//only get right side if it's a variable
				String* name2 = (String*)processTree(expr->right, TEXTEXPR);
			}
			else{ String* name2 = nullptr; }
			int32_t* val2 = (int32_t*)processTree(expr->right, NUMEXPR);
			Variable var = { *name1, *val2};			//create the variable
			if (isVariable(var)){					//if variable already exists, print error message
				printf("WARNING: variable %s incorrectly re-initialized\n", var.name.c_str());
				//reassign
				reassignVariable(var);
				return 0;
			}
			else{	//variable does not exist yet
				VariableList.push_back(var);
				return 0;
			}
		}
		else if (expr->name == String{ "set" }){		//reassign a variable
			//get all possible values (variables on both nodes)
			String* name1 = (String*)processTree(expr->left, TEXTEXPR);
			int32_t* val1 = (int32_t*)processTree(expr->left, NUMEXPR);
			if (expr->right->type == VARIABLE){		//only get right side if it's a variable
				String* name2 = (String*)processTree(expr->right, TEXTEXPR);
			}
			else{ String* name2 = nullptr; }
			int32_t* val2 = (int32_t*)processTree(expr->right, NUMEXPR);
			Variable var = { *name1, *val2 };			//create the variable
			if (!isVariable(var)){		//if variable does not exist, print warning
				printf("WARNING: variable %s not declared\n", var.name.c_str());
				//initialize
				VariableList.push_back(var);
				return 0;
			}
			else{	//reassign
				reassignVariable(var);
				return 0;
			}
		}
		//result will be from LEFT NODE, binary operators
		else{
			int* a = (int*)processTree(expr->left, NUMEXPR);
			int* b = (int*)processTree(expr->right, NUMEXPR);
			if (expr->name == String{ "+" }){ *b = *a + *b; }
			else if (expr->name == String{ "-" }){ *b = *a - *b; }
			else if (expr->name == String{ "*" }){ *b = *a * *b; }
			else if (expr->name == String{ "/" }){ *b = *a / *b; }
			else if (expr->name == String{ "%" }){ *b = *a % *b; }
			else if (expr->name == String{ "&&" }){ *b = *a && *b; }
			else if (expr->name == String{ "||" }){ *b = *a || *b; }
			else if (expr->name == String{ "<" }){ *b = *a < *b; }
			else if (expr->name == String{ ">" }){ *b = *a > *b; }
			else if (expr->name == String{ "==" }){ *b = *a == *b; }
			else if (expr->name == String{ "!=" }){ *b = *a != *b; }
			else if (expr->name == String{ "<=" }){ *b = *a <= *b; }
			else if (expr->name == String{ ">=" }){ *b = *a >= *b; }
			return b;
		}
	}
	else if (expr->type == UNARY){	//operands are on left node
		if (expr->name == String{ "!" }){	//bool opposite
			int* result = (int*)processTree(expr->left, NUMEXPR);
			if (*result){		//not 0, so make 0
				*result = 0;
				return result;
			}
			else{		//0, so make 1
				*result = 1;
				return result;
			}
		}
		else if (expr->name == String{ "~" }){	//arithmetic negation
			int* result = (int*)processTree(expr->left, NUMEXPR);
			*result *= -1;
			return result;
		}
	}
}
void destroyTree(Expr* root){		//delete the tree on the heap
	if (!root){ return; }
	destroyTree(root->left);
	destroyTree(root->right);
	delete root;
}
void processToken(void){
	while (true){
		Expr* root = new Expr{};
		root = root->insert();
		buildTree(root, NULLEXPR);					//build the parse tree with the root
		processTree(root, NULLEXPR);					//process tree
		destroyTree(root);
		if (next_token_type == END){
			clearVariables();
			printf("\n\n");
			return;
		}
	}


}
