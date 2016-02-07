#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <memory>
#include <cstdlib>

#include "Vector.h"

using std::cout;
class Foo {
public:
	Foo(void) { cout << "no-arg construction\n"; }
	Foo(int) { cout << "int constructor\n"; }
	Foo(Foo const&) { cout << "copy constructor\n"; }
	Foo(Foo&&) { cout << "move constructor\n"; }
	~Foo(void) { cout << "destructor\n"; }
};

void core_cpp_syntax(void) {
	Foo* p = (Foo*) ::operator new(10 * sizeof(Foo));
	new (p) Foo{42}; // placement new, runs the constructor (does not allocate)
	int k = 0;
	p[k].~Foo(); // run destructor (fairly straightforward)
	::operator delete(p); // deallocate memory (no destructors) 
}

//template <typename T>
//struct allocator {
//	T* allocate(uint64_t num) const {
//		return (T*) ::operator new(num * sizeof(T));
//	}
//	void deallocate(T* p) const {
//		::operator delete(p);
//	}
//
//	void construct(T* p) const {
//		new(p) T{};
//	}
//	void construct(T* p, T const& lref) const { 
//		new(p) T{ lref };
//	}
//
//	void destroy(T* p) const { p->~T(); }
//};

using std::allocator; 
void via_std_allocator(void) {
	allocator<Foo> my_alloc{};
	Foo* p = my_alloc.allocate(10);
	allocator<Foo>{}.construct(p);
	my_alloc.destroy(p);
	allocator<Foo>{}.deallocate(p, std::rand());
}

struct my_except {
	my_except(const char* msg) { this->msg = msg; }
	my_except(my_except& rhs) {
		msg = rhs.msg;
		cout << "copied exception\n";
	}
	const char* msg;
	const char* what(void) const { return msg; }
};

void doit(int k) {
	Foo f;
	if (k < 0 || k >= 10) {
		throw my_except{ "oops" };
	}
}

int main(void) {
	try {
		doit(42);
	} catch (my_except const & x) {
		cout << "you messed up " << x.what() << std::endl;
	}
}

