#include "Valarray.h"

using std::cout;
using std::endl;

int main(void) {
    complex<double> di{ 1.0, 2.0 };
    std::plus<complex<double>> op{};
    auto res = op(di, 2);
    cout << typeid(res).name() << endl;
    cout << res << endl;
    valarray<int> x = { 1, 2, 3, 4 };
    cout << x + di << endl;
    valarray<double> y{ 2.2, 3.3, 4.4, 5.5 };
    for (auto const & p : x.apply(std::negate<>{})) {
        cout << p << endl;; // p should be a double
    }
    auto p = (x + y).begin();
    auto q = (x + y).end();
    while (p != q) {
        cout << *p << endl;;
        ++p;
    }
}
