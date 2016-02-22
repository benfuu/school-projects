#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "Vector.h"

using std::cout;
using std::endl;
using std::allocator;
using epl::vector;

int main(void) {
	try {
		vector<int> x;
		for (int i = 0; i < 10; i++) {
			x.push_back(i);
			x.push_front(x[i] + 1);
			// x.push_front(i);
			// x.pop_front();
			// x.pop_back();
		}
		cout << "size of vector is " << x.size() << endl;
		cout << "capacity of vector is " << x.capacity() << endl;
		cout << "the start of the vector is element " << x.first_elem() << endl;
		cout << '[';
		for (int i = 0; i < x.size(); i++) {
			cout << x[i];
			if (i != x.size() - 1) {
				cout << ", ";
			}
		}
		cout << ']' << endl;
	}
	catch (std::out_of_range e) {
		cout << "exception: " << e.what() << endl;
	}
	
}