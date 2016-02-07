#include <iostream>
#include <type_traits>

using std::cout;
using std::endl;

class Complex {
public:
	double real;
	double imag;
	Complex(void) = default;
	~Complex(void) = default;
	Complex(Complex const & that) = default;

	Complex& operator=(Complex const & rhs) = default;
};

void bar(Complex param) {

}

int main(void) {
	cout << "Hello World\n";

	bool val = std::is_pod<Complex>::value;
	cout << std::boolalpha;
	cout << val << endl;

	Complex val1 = { 1.0, 0.0 };
	Complex val2(val1);
	bar(val1);
}

//Complex foo(void) {
//	double real_val = 42;
//	double imag_val = 10;
//
//	return{ real_val, imag_val };
//
//}
