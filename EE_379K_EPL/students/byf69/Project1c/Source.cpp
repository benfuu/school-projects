#include "Vector.h"
#include <cstdio>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::begin;
using std::end;
using epl::vector;

int main(void) {
    vector<int> x{ 10, 13, 15, 12, 10, 0, 1, 2, 45 };
    vector<int>::iterator it = x.begin();
    //x.emplace_back(30);
    vector<int>::const_iterator const_it = x.begin();
    const_it += 3;
    vector<int>::iterator b = x.begin();
    vector<int>::const_iterator c = x.begin() + 3;
    c++;
    c += 2;
    auto d = x.begin();
    cout << "difference is " << c - (vector<int>::iterator) x.begin() << endl;;
    cout << *const_it << endl;
    const_it = it;
    vector<int>::const_iterator it1{ const_it };
    for (auto i = begin(x); i != end(x); i++) {
        cout << *i << endl;
    }
}