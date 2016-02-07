// Constructors/Source.cpp -- everything you always wanted to know about Constructors (and destructors)

#include <iostream>
#include <cstdint>

using std::cout;
using std::endl;

/*
 * Some initial notes:
 * The purpose of a constructor is to ensure that class instances (i.e., "objects") are initialized 
 * correctly and consistently with the design-intent for that class. Most notably, initialization should
 * ensure that an object satisfies any and all design invariants for the class and that the object is a
 * a "reasonable object". The meaning of "reasonable" is highly context dependent, but the rule of thumb that
 * a designer should use is this
 *
 *   If an application developer can declare a variable or obtain an object in any way, then that object
 *   should behave in a reasonable and predicable way
 *
 * So, for example, consider a simple String class
 */

class String1 {
	char* data; // pointer to character data stored on the heap
	uint32_t length; // number of characters in the string
public:
	String1(void) = default;
	String1(char const*);
};

/* in this string, I've retained the default initialization and created a second constructor. Default initialiation will 
 * leave the data pointer and length fields with random values -- i.e, the pointer will point to a random
 * location in memory -- this initialization would undoubtedly produce unpredicatable behavior in programs. I would 
 * consider String1 to be poorly designed if it retained the default constructor
 *
 * It's OK if you provide a default initialization that assigns your object a value where the object is invalid and 
 * will immediately throw an exception if it's used -- we'll do that when we create iterator objects, for example.
 * As long as the behavior is predictable (and ideally documented in your API documentation), then your initialization
 * can be considered reasonable
 *
 * As an example, when I build a String class, I always provide a default (zero-argument) constructor that initializes
 * the object to be an empty string -- i.e., ""
 */


/* Type conversion (promotion) via a constructor 
 * C++ has very powerful (some would say "aggressive") support for automatically and silently converting objects
 * from one type into another. The compiler will auto convert the type of an object when evaluating an expression
 * performing an assignment or when passing an argument to a function. We think nothing of this in C when an
 * integer value is silently and automatically converted to double in an expression such as 3.5 + 1
 * We'll eventually be faced with several examples where C++ does NOT automatically convert types (e.g,. template
 * argument deduction), but for now, if the compiler can't complete the operation you've asked it to do, and if
 * the compiler can find an available type conversion, it will convert the types (without telling you). 
 *
 * Constructors can be type conversion operators. Specifically, any non-explicit constructor with exactly one
 * parameter where the parameter is a different type than the class type is a valid type conversion function
 * (recall that a constructor with exactlyone argument that is the same as the class type is the copy constructor)
 * In the simple String class above the constructor
 *
 *   String::String(char const*);
 *
 * is a type conversion operator. The compiler will automatically (and silently) invoke this constructor whenever
 * it sees an expression where the current type of the expression is char* (or char const*) and the compiler needs 
 * the expression to be type String.
 *
 * Consider the following
 */

class String2 {
	/* object contents are unimportant, so left blank for this demonstration */
public:
	String2(void) {}
	String2(char const* s) { cout << "initialization via " << s << endl;  }
};

String2 concat(String2 s1, String2 s2) {
	String2 result;
	/* set result to be the concatenation of s1 and s2 */
	return result;
}

void conversionDemo(void) {
	String2 val = concat("Hello", "World");
}

/* Note that the invocation of the concat function has two char const* expressions (string literals 
 * in C++ like "Hello" are pointers which point to read-only character strings, hence char const*)
 * yet the concat function itself requires parameters of type String2. The compiler resolves the
 * type difference by converting (silently) the char const* arguments into String2 parameters using
 * the String2::String2(char const*) constructor
 *
 *
 *
 *
 * For some classes, we want to provide a single-argument constructor, but yet we don't want that
 * constructor to be available as a type conversion operator. In those cases, we decorate our 
 * single-argument constructor with the prefix "explicit". Only single-argument constructors
 * need to be made explicit, and only those constructors where the behavior of the initialization
 * is incompatible with type conversion -- i.e., there's no reason that either of the constructors
 * for my string class need to be explicit. However, for Project1, the vector class does have a
 * constructor that should be made explicit. Here's a simplified excerpt
 */

class vector {
public:
	explicit vector(int n) {
		/* initiailizes the vector using the argument to indicate the number of elemetns in the vector
		 * if n were 10, for example, the vector would be initialized to hold ten elements, each element
		 * would be initialized to the default value (e.g., 0)
		 * we make this constructor explicit so that we don't inadvertently create a conversion operator
		 * that silently converts integers into vectors */
	}
};

int main(void) {
	conversionDemo();
}