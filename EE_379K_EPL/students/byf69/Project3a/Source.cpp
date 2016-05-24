#include "Valarray.h"

using std::cout;
using std::endl;

int main(void) {
    /*complex<double> di{ 1.0, 2.0 };
    std::plus<complex<double>> op{};
    auto res = op(di, 2);
    cout << typeid(res).name() << endl;
    cout << res << endl;*/
    valarray<int> x(10);
    valarray<int> y(20);
    cout << ((2 * (1 + x - y + 1)) / 1) << endl;
    valarray<int> z = (2 * (1 + x - y + 1)) / 1;
    z = -z;
    cout << z << endl;
    x = -((2 * (1 + x - y + 1)) / 1);
    cout << x << endl;
    z *= z;
    cout << z << endl;
}
