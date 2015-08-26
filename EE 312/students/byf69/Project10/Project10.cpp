#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "Vector.h"
#include "String.h"
#include "Parse.h"
#include "Expr.h"

Vector<Variable> VariableList{};					//symbol table
int globalnum[10000];
int globalpos = 0;
bool odflag = false;
void buildExprTree(Expr* expr, ExprType type, bool read);
void buildStmtTree(Stmt* stmt, StmtType type, bool read);
void* processExprTree(Expr* expr, ExprType type);
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
void buildExprTree(Expr* expr, ExprType type, bool read){
	if (read == true){ read_next_token(); }				//read next input	
	if (next_token_type == NUMBER){					//set value to the number, don't build any more nodes
		expr->insert(token_number_value);
		return;
	}
	else if (next_token_type == STRING || type == STRINGEXPR){	//copy string, don't build any more nodes
		expr->insert(next_token());
		return;
	}
	else if (next_token_type == SYMBOL){
		if (String{ next_token() } == String{ "!" } ||
			String{ next_token() } == String{ "~" }){			//unary operators, only build left side
			expr->insert(UNARY, next_token());
			buildExprTree(expr->left, NUMEXPR, true);
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
			buildExprTree(expr->left, NUMEXPR, true);
			buildExprTree(expr->right, NUMEXPR, true);
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
			buildExprTree(expr->left, STRINGEXPR, true);
		}
		else if (String{ next_token() } == String{ "output" }){
			expr->insert(TEXTEXPR, next_token());
			buildExprTree(expr->left, NUMEXPR, true);
		}
		else if (String{ next_token() } == String{ "var" } ||
			String{ next_token() } == String{ "set" }){
			//variable commands are just binary =, variable goes on left node and value goes on right node
			expr->insert(BINARY, next_token());
			buildExprTree(expr->left, VARIABLE, true);		//SPECIAL FLAG for variable call
			buildExprTree(expr->right, NUMEXPR, true);
		}
		//THIS SECTION IS FOR STMTS
		else if (String{ next_token() } == String{ "do" } ||
			String{ next_token() } == String{ "od" }){
			//all statements
			Stmt* root = new Stmt{};
			buildStmtTree(root, NULLSTMT, false);
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
void buildStmtTree(Stmt* stmt, StmtType type, bool read){
	if (read == true){ read_next_token(); }
	//do loops
	if (String{ next_token() } == String{ "do" } ||
		String{ next_token() } == String{ "od" } ||
		type == DO || type == DOAGAIN || type == OD){

		if (String{ next_token() } == String{ "od" }){	//if it was OD, do nothing
			if (stmt->type == NULLSTMT){ stmt->type = OD; }
			if (stmt->body1.size() && stmt->body1[stmt->body1.size() - 1]->body1.size() &&
				stmt->body1[stmt->body1.size() - 1]->body1[stmt->body1[stmt->body1.size() - 1]->body1.size() - 1]->type == OD){
			}
			else{ return; }
		}
		//construct a DO loop
		if (type != DOAGAIN && type != IFAGAIN && type != ELSEAGAIN){	//only create a new statement if do has not been called yet
			stmt->insert(DO);								//create a struct with type DO
			buildExprTree(stmt->expr, NULLEXPR, true);		//build an expression tree for the condition
		}
		Stmt* block = new Stmt{};						//create a new statement on the heap
		buildStmtTree(block, NULLSTMT, true);			//build a statement tree	
		stmt->body1.push_back(block);					//push statment into the vector
		if (block->type == FI){ buildStmtTree(stmt, IFAGAIN, false); }	//if do was nested in if, this will return to the if loop
		else{ buildStmtTree(stmt, DOAGAIN, false); }

	}

	//if loops
	else if (String{ next_token() } == String{ "if" } ||
		String{ next_token() } == String{ "else" } ||
		String{ next_token() } == String{ "fi" } ||
		type == IF || type == IFAGAIN || type == ELSE || type == ELSEAGAIN || type == FI){

		if (String{ next_token() } == String{ "fi" }){	//if it was FI, do nothing
			if (stmt->type == NULLSTMT){stmt->type = FI; }
			if (stmt->body1.size() && stmt->body1[stmt->body1.size() - 1]->body1.size() &&
				stmt->body1[stmt->body1.size() - 1]->body1[stmt->body1[stmt->body1.size() - 1]->body1.size() - 1]->type == FI ||
				stmt->body1.size() && stmt->body1[stmt->body1.size() - 1]->body2.size() &&
				stmt->body1[stmt->body1.size() - 1]->body2[stmt->body1[stmt->body1.size() - 1]->body2.size() - 1]->type == FI){
			}
			else{ return; }
		}
		else if (String{ next_token() } == String{ "else" }){	//if it was ELSE, just break current loop
			if (	stmt->type == NULLSTMT){
				stmt->type = ELSE;
				return;
			}
			else if (stmt->body1.size() && stmt->body1[stmt->body1.size() - 1]->type == ELSE){
			}
		}
		//construct an IF loop
		if (type != IFAGAIN && type != ELSEAGAIN && type != DOAGAIN){	//only create a new statement if do has not been called yet
			stmt->insert(IF);								//create a struct with type IF
			buildExprTree(stmt->expr, NULLEXPR, true);		//build an expression tree for the condition
		}
		Stmt* block = new Stmt{};						//create a new statement on the heap
		buildStmtTree(block, NULLSTMT, true);			//build a statement tree	

		if (type == ELSEAGAIN || type == ELSE){
			stmt->body2.push_back(block);					//push statment into the vector
			if (block->type == OD){ buildStmtTree(stmt, DOAGAIN, false); }
			else{ buildStmtTree(stmt, ELSEAGAIN, false); }
		}
		else if (type == IFAGAIN || type == NULLSTMT || block->type == ELSE){
			stmt->body1.push_back(block);					//push statment into the vector
			if (block->type == ELSE){ buildStmtTree(stmt, ELSEAGAIN, false); }
			else if (block->type == OD){ buildStmtTree(stmt, DOAGAIN, false); }
			else{ buildStmtTree(stmt, IFAGAIN, false); }
		}
	}
	else{	//just a normal expression
		stmt->insert(EXPR);
		buildExprTree(stmt->expr, NULLEXPR, false);			//make sure NOT to read again
	}
	return;
}
void* processExprTree(Expr* expr, ExprType type){
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
			String* s = (String*)processExprTree(expr->left, NULLEXPR);
			printf("%s", s->c_str());
			//printf("\n");
		}
		else if (expr->name == String{ "output" }){	//evaluate expression on left
			int32_t* a = (int32_t*)processExprTree(expr->left, NUMEXPR);
			printf("%d", *a);
			//printf("\n");
		}
	}
	else if (expr->type == BINARY){
		if (expr->name == String{ "var" }){		//initialize a variable
			//get all possible values (variables on both nodes)
			String* name1 = (String*)processExprTree(expr->left, TEXTEXPR);
			int32_t* val1 = (int32_t*)processExprTree(expr->left, NUMEXPR);
			if (expr->right->type == VARIABLE){		//only get right side if it's a variable
				String* name2 = (String*)processExprTree(expr->right, TEXTEXPR);
			}
			else{ String* name2 = nullptr; }
			int32_t* val2 = (int32_t*)processExprTree(expr->right, NUMEXPR);
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
			String* name1 = (String*)processExprTree(expr->left, TEXTEXPR);
			int32_t* val1 = (int32_t*)processExprTree(expr->left, NUMEXPR);
			if (expr->right->type == VARIABLE){		//only get right side if it's a variable
				String* name2 = (String*)processExprTree(expr->right, TEXTEXPR);
			}
			else{ String* name2 = nullptr; }
			int32_t* val2 = (int32_t*)processExprTree(expr->right, NUMEXPR);
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
			int* a = (int*)processExprTree(expr->left, NUMEXPR);
			int* b = (int*)processExprTree(expr->right, NUMEXPR);
			globalpos += 1;
			if (expr->name == String{ "+" }){ globalnum[globalpos] = *a + *b; }
			else if (expr->name == String{ "-" }){ globalnum[globalpos] = *a - *b; }
			else if (expr->name == String{ "*" }){ globalnum[globalpos] = *a * *b; }
			else if (expr->name == String{ "/" }){ globalnum[globalpos] = *a / *b; }
			else if (expr->name == String{ "%" }){ globalnum[globalpos] = *a % *b; }
			else if (expr->name == String{ "&&" }){ globalnum[globalpos] = *a && *b; }
			else if (expr->name == String{ "||" }){ globalnum[globalpos] = *a || *b; }
			else if (expr->name == String{ "<" }){ globalnum[globalpos] = *a < *b; }
			else if (expr->name == String{ ">" }){ globalnum[globalpos] = *a > *b; }
			else if (expr->name == String{ "==" }){ globalnum[globalpos] = *a == *b; }
			else if (expr->name == String{ "!=" }){ globalnum[globalpos] = *a != *b; }
			else if (expr->name == String{ "<=" }){ globalnum[globalpos] = *a <= *b; }
			else if (expr->name == String{ ">=" }){ globalnum[globalpos] = *a >= *b; }
			return &globalnum[globalpos];
		}
	}
	else if (expr->type == UNARY){	//operands are on left node
		if (expr->name == String{ "!" }){	//bool opposite
			int* result = (int*)processExprTree(expr->left, NUMEXPR);
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
			int* result = (int*)processExprTree(expr->left, NUMEXPR);
			*result *= -1;
			return result;
		}
	}
}
void* processStmtTree(Stmt* stmt, StmtType type){
	if (stmt->type == DO || stmt->type == DOAGAIN || stmt->type == OD){
		if (stmt->type == OD){ return 0; }
		while (*(bool*)processExprTree(stmt->expr, NULLEXPR)){	//evaluate expression only if false
			for (uint32_t i = 0; i < stmt->body1.size(); i += 1){	//evaluate and execute all the statements
				processStmtTree(stmt->body1[i], EXPR);
			}
		}
	}
	else if (stmt->type == IF || stmt->type == IFAGAIN || stmt->type == ELSE || stmt->type == ELSEAGAIN || stmt->type == FI){
		if (stmt->type == FI || stmt->type == ELSE){ return 0; }
		if (*(bool*)processExprTree(stmt->expr, NULLEXPR)){	//evaluate if expression only if true
			for (uint32_t i = 0; i < stmt->body1.size(); i += 1){	//evaluate and execute all the statements
				processStmtTree(stmt->body1[i], EXPR);
			}
		}
		else{
			for (uint32_t i = 0; i < stmt->body2.size(); i += 1){	//evaluate and execute all the else statements
				processStmtTree(stmt->body2[i], EXPR);
			}
		}
	}
	else if (stmt->type == EXPR){		//if statement is JUST an expression
		processExprTree(stmt->expr, NULLEXPR);
		return 0;						//doesn't matter what you return
	}
}
void destroyExprTree(Expr* root){		//delete the tree on the heap
	if (!root){ return; }
	destroyExprTree(root->left);
	destroyExprTree(root->right);
	delete root;
}
void destroyStmtTree(Stmt* root){
	if (!root){ return; }
	destroyExprTree(root->expr);
	for (uint32_t i = 0; i < root->body1.size(); i += 1){
		destroyStmtTree(root->body1[i]);
		root->body1.pop_back();
	}
	for (uint32_t i = 0; i < root->body2.size(); i += 1){
		destroyStmtTree(root->body2[i]);
		root->body2.pop_back();
	}
	delete root;
}
void processToken(void){
	while (true){
		Stmt* sroot = new Stmt{};
		buildStmtTree(sroot, NULLSTMT, true);		//build the parse tree with the root
		processStmtTree(sroot, NULLSTMT);			//process tree
		destroyStmtTree(sroot);
		if (next_token_type == END){
			clearVariables();
			printf("\n\n");
			return;
		}
	}


}
