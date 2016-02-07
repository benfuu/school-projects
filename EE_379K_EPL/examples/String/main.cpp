/*
 * main.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: chase
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include "String.h"

using std::cout;
using std::endl;
using std::vector;


//void test_compare(void) {
//	vector<String> v{"abcdefgh", "abcdefg", "abcdefghi", ""};
//	std::sort(v.begin(), v.end());
//	for (const auto& s : v) {
//		cout << s << endl;
//	}
//}
//
//void doit(String t) {
//	cout << t << endl;
//	t = "overwritten";
//	cout << t << endl;
//}

bool operator!=(const String& x, const String& y) {
	return !( x == y);
}

int main(void) {
	String s{"Hello World"};
	if (s != "Hello World") {

	}

	s[4] = 'd';

}



