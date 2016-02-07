#include <iostream>
#include <cstdint>

#include "String.h"

using std::cout;
using std::endl;


void doit(String param) {
//	cout << param[1];
}

int main(void) {
	String s1{ "Hello World" };

	s1.operator[](0) = 'J';
	s1[1] = 'a';

//	doit(s1);
	String s2{ s1 + "!" };

	String tmp{ " Cool beans." };
	s2 = s2 + tmp;

	for (uint32_t k = 0; k < s2.size(); k += 1) {
		cout << s2[k];
	}
	cout << endl;
}