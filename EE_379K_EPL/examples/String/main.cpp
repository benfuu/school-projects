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
#include <utility>

using std::cout;
using std::endl;
using std::vector;


// I have little luck with rel_ops
//using namespace std::rel_ops;

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

//bool operator!=(const String& x, const String& y) {
//	return !( x == y);
//}

template <typename T>
bool operator!=(T const& x, T const& y) {
	return !(x == y);
}

int main(void) {
	String s{"Hello World"};

	String::iterator b = s.begin();
	auto e = s.end();
	while (b != e) {
		cout << *b;
		++b;
		s = String{ "gotcha" };
	}
	cout << endl;
}



