//Expr.h - definitions for the Expr struct and functions for project 9
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "Parse.h"
#include "String.h"

struct Variable{
	String name;
	int32_t value;
};

enum ExprType{
	TEXTEXPR,
	VARIABLE,
	NUMEXPR,
	STRINGEXPR,
	BINARY,
	UNARY,
	NULLEXPR
};
struct Expr{
	ExprType type;
	Expr* left;
	Expr* right;
	String name;
	int32_t value;
	Expr(void){
		type = NULLEXPR;
		left = nullptr;
		right = nullptr;
		name = String{};
		value = 0;
	}
	Expr(ExprType expr_type, const char* typeval){		//expressions (text,output,var,set)
		type = expr_type;
		left = nullptr;
		right = nullptr;
		name = String{ typeval };
		value = 0;
	}
	Expr(int32_t num){								//numbers
		type = NUMEXPR;
		left = nullptr;
		right = nullptr;
		name = String{};
		value = num;
	}
	Expr(const char* string){						//text and strings
		type = STRINGEXPR;
		left = nullptr;
		right = nullptr;
		name = String{ string };
		value = 0;
	}
	~Expr(void){
		/*type = INVALID;
		type_value = String{};
		left = nullptr;
		right = nullptr;
		name = String{};
		value = 0;*/
	}

	//functions for inserting nodes
	Expr* insert(void){
		Expr* node = new Expr{};
		return node;
	}
	Expr* insert(ExprType expr_type, const char* typeval){		//for inserting expressions
		*this = Expr{ expr_type, next_token() };		//assign correct values to this
		if (expr_type == BINARY){	//binary operator, create left and right nodes
			this->left = new Expr{};
			this->right = new Expr{};
		}
		else if (expr_type == UNARY || expr_type == TEXTEXPR || expr_type == VARIABLE){
			this->left = new Expr{};	//just create left node
		}
		return this;
	}
	Expr* insert(int32_t num){									//for inserting numbers
		*this = Expr{ num };
		return this;
	}
	Expr* insert(const char* string){							//for inserting strings
		*this = Expr{ string };
		return this;
	}
};

enum StmtType{
	EXPR,	//normal expression
	DO,	DOAGAIN, OD,	//do statements
	IF,	IFAGAIN, ELSE, ELSEAGAIN, FI,	//if statement
	NULLSTMT
};
struct Stmt{
	StmtType type;
	Expr* expr;
	Vector<Stmt*> body1;
	Vector<Stmt*> body2;
	String name;

	Stmt(void){
		type = NULLSTMT;
		expr = nullptr;
		body1 = Vector < Stmt* > {};
		body2 = Vector < Stmt* > {};
		name = String{};
	}
	Stmt(StmtType stype){
		type = stype;
		expr = nullptr;
		body1 = Vector < Stmt* > {};
		body2 = Vector < Stmt* > {};
	}
	Stmt* insert(void){
		Stmt* node = new Stmt{};
		return node;
	}
	void insert(StmtType stype){
		*this = Stmt{ stype };
		this->expr = new Expr{};
	}
};



void processToken(void);