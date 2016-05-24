// Valarray.h

/* Put your solution in this file, we expect to be able to use
 * your epl::valarray class by simply saying #include "Valarray.h"
 *
 * We will #include "Vector.h" to get the epl::vector<T> class 
 * before we #include "Valarray.h". You are encouraged to test
 * and develop your class using std::vector<T> as the base class
 * for your epl::valarray<T>
 * you are required to submit your project with epl::vector<T>
 * as the base class for your epl::valarray<T>
 */
#pragma once
#ifndef _Valarray_h
#define _Valarray_h

#include <iostream>
#include <cstdlib>
//#include <vector>
#include <cstdint>
#include <algorithm>
#include <complex>
#include <functional>
#include "Vector.h"

using std::cout;
using std::endl;
using std::complex;

//using std::vector; // during development and testing
using epl::vector; // after submission


template <typename T>
class valarray;

template <typename E1, typename E2, typename Op>
class ValarrayExpr;

template <typename N>
class Number;


/* Type Ranks */
template <typename T>
constexpr T get_max_rank(T const & x, T const & y) {
    return x < y ? y : x;
}

template <typename>
struct SRank;
template <> struct SRank<int> {
    static constexpr int value = 1;
    static constexpr bool is_complex = false;
};
template <> struct SRank<float> {
    static constexpr int value = 2;
    static constexpr bool is_complex = false;
};
template <> struct SRank<double> {
    static constexpr int value = 3;
    static constexpr bool is_complex = false;
};
template <typename T> struct SRank<valarray<T>> {
    static constexpr int value = SRank<T>::value;
    static constexpr bool is_complex = SRank<T>::is_complex;
};
template <typename E1, typename E2, typename Op> struct SRank<ValarrayExpr<E1, E2, Op>> {
    static constexpr int value = get_max_rank(SRank<typename E1::value_type>::value, SRank<typename E2::value_type>::value);
    static constexpr bool is_complex = SRank<typename E1::value_type>::is_complex | SRank<typename E2::value_type>::is_complex;
};
template <typename N> struct SRank<Number<N>> {
    static constexpr int value = SRank<N>::value;
    static constexpr bool is_complex = SRank<N>::is_complex;
};
template <typename T> struct SRank<complex<T>> {
    static constexpr int value = SRank<T>::value;
    static constexpr bool is_complex = true;
};
/*
template <typename T> struct SRank<valarray<complex<T>>> {
    static constexpr int value = SRank<T>::value;
    static constexpr bool is_complex = true;
};
template <typename N> struct SRank<Number<N>> {
    static constexpr int value = SRank<N>::value;
    static constexpr bool is_complex = false;
};
*/

template <int, bool>
struct SType;
template <> struct SType<1, false> { using type = int; };
template <> struct SType<2, false> { using type = float; };
template <> struct SType<2, true> { using type = complex<float>; };
template <> struct SType<3, false> { using type = double; };
template <> struct SType<3, true> { using type = complex<double>; };

template <typename T1, typename T2>
struct choose_type {
    static constexpr int t1_rank = SRank<T1>::value;
    static constexpr bool t1_is_complex = SRank<T1>::is_complex;
    static constexpr int t2_rank = SRank<T2>::value;
    static constexpr bool t2_is_complex = SRank<T2>::is_complex;
    static constexpr int max_rank = get_max_rank(t1_rank, t2_rank);
    static constexpr bool is_complex = t1_is_complex | t2_is_complex;
    using type = typename SType<max_rank, is_complex>::type;
};

template <typename T1, typename T2>
using ChooseType = typename choose_type<T1, T2>::type;

//template <typename T1, typename T2>
//ChooseType<T1, T2> get_max_rank(T1 const & x, T2 const & y) {
//    if (x < y) { return y; }
//    else { return x; }
//}



/* Reference Template */
template <typename T>
struct choose_ref {
    using type = T;
};

template <typename T>
struct choose_ref<valarray<T>> {
    using type = valarray<T> const &;
};

template <typename T>
using ChooseRef = typename choose_ref<T>::type;


/* Number Wrapper Class */
template <typename N>
class Number {
    N m_val;
    uint64_t m_size = std::numeric_limits<uint64_t>::max();
public:
    using value_type = N;
    Number(Number const &);
    Number(N const &);
    N operator[](uint64_t) const;
    uint64_t size() const;
    /* Operator + */
    template <typename N1, typename T>
    friend ValarrayExpr<Number<N1>, valarray<T>, std::plus<ChooseType<N1, T>>> operator+(N1 const &, valarray<T> const &); // number + valarray
    template <typename N1, typename E1, typename E2, typename Op>
    friend ValarrayExpr<Number<N1>, ValarrayExpr<E1, E2, Op>, std::plus<ChooseType<N1, ChooseType<typename E1::value_type, typename E2::value_type>>>>
        operator+(N1 const &, ValarrayExpr<E1, E2, Op> const &); // number + expr
    /* Operator - */
    template <typename N1, typename T>
    friend ValarrayExpr<Number<N1>, valarray<T>, std::minus<ChooseType<N1, T>>> operator-(N1 const &, valarray<T> const &); // number - valarray
    template <typename N1, typename E1, typename E2, typename Op>
    friend ValarrayExpr<Number<N1>, ValarrayExpr<E1, E2, Op>, std::minus<ChooseType<N1, ChooseType<typename E1::value_type, typename E2::value_type>>>>
        operator-(N1 const &, ValarrayExpr<E1, E2, Op> const &); // number - expr
    /* Operator * */
    template <typename N1, typename T>
    friend ValarrayExpr<Number<N1>, valarray<T>, std::multiplies<ChooseType<N1, T>>> operator*(N1 const &, valarray<T> const &); // number * valarray
    template <typename N1, typename E1, typename E2, typename Op>
    friend ValarrayExpr<Number<N1>, ValarrayExpr<E1, E2, Op>, std::multiplies<ChooseType<N1, ChooseType<typename E1::value_type, typename E2::value_type>>>>
        operator*(N1 const &, ValarrayExpr<E1, E2, Op> const &); // number * expr
    /* Operator / */
    template <typename N1, typename T>
    friend ValarrayExpr<Number<N1>, valarray<T>, std::divides<ChooseType<N1, T>>> operator/(N1 const &, valarray<T> const &); // number / valarray
    template <typename N1, typename E1, typename E2, typename Op>
    friend ValarrayExpr<Number<N1>, ValarrayExpr<E1, E2, Op>, std::divides<ChooseType<N1, ChooseType<typename E1::value_type, typename E2::value_type>>>>
        operator/(N1 const &, ValarrayExpr<E1, E2, Op> const &); // number / expr
};


/* Valarray Expression Class */
template <typename E1, typename E2, typename Op>
class ValarrayExpr {
private:
    using E1Type = ChooseRef<E1>;
    using E2Type = ChooseRef<E2>;
    E1Type m_arr1;
    E2Type m_arr2;
    uint64_t m_size;
    Op op;
public:
    using value_type = ChooseType<typename E1::value_type, typename E2::value_type>;
    ValarrayExpr(E1 const &, E2 const &, Op);
    ChooseType<typename E1::value_type, typename E2::value_type> operator[](uint64_t) const;
    uint64_t size() const;
    /* Operator + */
    template <typename T>
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, valarray<T>,
        std::plus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>>
        operator+(valarray<T> const &); // expr + valarray
    template <typename F1, typename F2, typename FOp>
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
        std::plus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>> >>
        operator+(ValarrayExpr<F1, F2, FOp> const &);   // expr + expr
    template <typename N>
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, Number<N>,
        std::plus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>>
        operator+(N const &);   // expr + number
    /* Operator - */
    template <typename T>
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, valarray<T>,
        std::minus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>>
        operator-(valarray<T> const &); // expr - valarray
    template <typename F1, typename F2, typename FOp>
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
        std::minus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>> >>
        operator-(ValarrayExpr<F1, F2, FOp> const &);   // expr - expr
    template <typename N>
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, Number<N>,
        std::minus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>>
        operator-(N const &);   // expr - number
    /* Operator * */
    template <typename T>
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, valarray<T>,
        std::multiplies<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>>
        operator*(valarray<T> const &); // expr * valarray
    template <typename F1, typename F2, typename FOp>
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
        std::multiplies<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>> >>
        operator*(ValarrayExpr<F1, F2, FOp> const &);   // expr * expr
    template <typename N>
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, Number<N>,
        std::multiplies<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>>
        operator*(N const &);   // expr * number
    /* Operator / */
    template <typename T>
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, valarray<T>,
        std::divides<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>>
        operator/(valarray<T> const &); // expr / valarray
    template <typename F1, typename F2, typename FOp>
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
        std::divides<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>> >>
        operator/(ValarrayExpr<F1, F2, FOp> const &);   // expr / expr
    template <typename N>
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, Number<N>,
        std::divides<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>>
        operator/(N const &);   // expr / number
    /* Unary Operator - */
    ValarrayExpr<Number<int>, ValarrayExpr<E1, E2, Op>, std::minus<typename ValarrayExpr<E1, E2, Op>::value_type>> operator-();   // - expr
};


/* Valarray Class */
template <typename T>
class valarray : public vector<T> {
public:
    using vector<T>::vector;    // inherit vector constructors
    valarray();
    template <typename T2>
    valarray(valarray<T2> const &); // conversion/copy constructor
    template <typename E1, typename E2, typename Op>
    valarray(ValarrayExpr<E1, E2, Op> const &);
    template <typename T2>
    operator vector<T2>() const;
    template <typename T2>
    valarray<ChooseType<T, T2>> & operator=(valarray<T2> const &);
    template <typename E1, typename E2, typename Op>
    valarray<ChooseType<typename E1::value_type, typename E2::value_type>> & operator=(ValarrayExpr<E1, E2, Op> const &);
    template <typename N>
    valarray<ChooseType<T, N>> & operator=(N const &);
    /* Operator += */
    template <typename T2>
    valarray<ChooseType<T, T2>> & operator+=(valarray<T2> const &);    // valarray += valarray or expr
    template <typename E1, typename E2, typename Op>
    valarray<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>> & operator+=(ValarrayExpr<E1, E2, Op> const &);   // valarray += expr
    template <typename N>
    valarray<ChooseType<T, N>> & operator+=(N const &); // valarray += number
    /* Operator -= */
    template <typename T2>
    valarray<ChooseType<T, T2>> & operator-=(valarray<T2> const &);    // valarray -= valarray or expr
    template <typename E1, typename E2, typename Op>
    valarray<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>> & operator-=(ValarrayExpr<E1, E2, Op> const &);   // valarray -= expr
    template <typename N>
    valarray<ChooseType<T, N>> & operator-=(N const &); // valarray -= number
    /* Operator *= */
    template <typename T2>
    valarray<ChooseType<T, T2>> & operator*=(valarray<T2> const &);    // valarray *= valarray or expr
    template <typename E1, typename E2, typename Op>
    valarray<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>> & operator*=(ValarrayExpr<E1, E2, Op> const &);   // valarray *= expr
    template <typename N>
    valarray<ChooseType<T, N>> & operator*=(N const &); // valarray *= number
    /* Operator /= */
    template <typename T2>
    valarray<ChooseType<T, T2>> & operator/=(valarray<T2> const &);    // valarray /= valarray or expr
    template <typename E1, typename E2, typename Op>
    valarray<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>> & operator/=(ValarrayExpr<E1, E2, Op> const &);   // valarray /= expr
    template <typename N>
    valarray<ChooseType<T, N>> & operator/=(N const &); // valarray /= number
    /* Operator + */
    template <typename T2>
    ValarrayExpr<valarray<T>, valarray<T2>, std::plus<ChooseType<T, T2>>> operator+(valarray<T2> const &); // valarray + valarray
    template <typename E1, typename E2, typename Op>
    ValarrayExpr<valarray<T>, ValarrayExpr<E1, E2, Op>,
        std::plus<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>>
        operator+(ValarrayExpr<E1, E2, Op> const &);    // valarray + expr
    template <typename N>
    ValarrayExpr<valarray<T>, Number<N>, std::plus<ChooseType<T, N>>> operator+(N const &);    // valarray + number
    /* Operator - */
    template <typename T2>
    ValarrayExpr<valarray<T>, valarray<T2>, std::minus<ChooseType<T, T2>>> operator-(valarray<T2> const &); // valarray - valarray
    template <typename E1, typename E2, typename Op>
    ValarrayExpr<valarray<T>, ValarrayExpr<E1, E2, Op>,
        std::minus<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>> >>
        operator-(ValarrayExpr<E1, E2, Op> const &);    // valarray - expr
    template <typename N>
    ValarrayExpr<valarray<T>, Number<N>, std::minus<ChooseType<T, N>>> operator-(N const &);    // valarray - number
    /* Operator * */
    template <typename T2>
    ValarrayExpr<valarray<T>, valarray<T2>, std::multiplies<ChooseType<T, T2>>> operator*(valarray<T2> const &); // valarray * valarray
    template <typename E1, typename E2, typename Op>
    ValarrayExpr<valarray<T>, ValarrayExpr<E1, E2, Op>,
        std::multiplies<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>> >>
        operator*(ValarrayExpr<E1, E2, Op> const &);    // valarray * expr
    template <typename N>
    ValarrayExpr<valarray<T>, Number<N>, std::multiplies<ChooseType<T, N>>> operator*(N const &);    // valarray * number
    /* Operator / */
    template <typename T2>
    ValarrayExpr<valarray<T>, valarray<T2>, std::divides<ChooseType<T, T2>>> operator/(valarray<T2> const &); // valarray / valarray
    template <typename E1, typename E2, typename Op>
    ValarrayExpr<valarray<T>, ValarrayExpr<E1, E2, Op>,
        std::divides<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>> >>
        operator/(ValarrayExpr<E1, E2, Op> const &);    // valarray / expr
    template <typename N>
    ValarrayExpr<valarray<T>, Number<N>, std::divides<ChooseType<T, N>>> operator/(N const &);    // valarray / number
    /* Unary Operator - */
    ValarrayExpr<Number<int>, valarray<T>, std::minus<T>> operator-(); // - valarray
};



/* Print Functions */
template <typename T>
std::ostream & operator<<(std::ostream & out, valarray<T> const & arr) {
    char const * sep = "";
    for (auto i = arr.begin(); i != arr.end(); i++) {
        out << sep << *i;
        sep = ", ";
    }
    return out;
}

template <typename E1, typename E2, typename Op>
std::ostream & operator<<(std::ostream & out, ValarrayExpr<E1, E2, Op> const & arr) {
    char const * sep = "";
    for (uint64_t i = 0; i < arr.size(); i++) {
        out << sep << arr[i];
        sep = ", ";
    }
    return out;
}

/*
 * Number Class
 */

template <typename N>
Number<N>::Number(Number<N> const & num) : m_val{ num.m_val } {}

template <typename N>
Number<N>::Number(N const & num) : m_val{num} {}

template <typename N>
N Number<N>::operator[](uint64_t k) const {
    return this->m_val;
}

template <typename N>
uint64_t Number<N>::size() const {
    return this->m_size;
}

/* Operator + */
template <typename N, typename T>
ValarrayExpr<Number<N>, valarray<T>, std::plus<ChooseType<N, T>>> operator+(N const & num, valarray<T> const & arr) {   // number + valarray
    ValarrayExpr<Number<N>, valarray<T>, std::plus<ChooseType<N, T>>> result(num, arr, std::plus<ChooseType<N, T>>());
    return result;
}
template <typename N, typename E1, typename E2, typename Op>
ValarrayExpr<Number<N>, ValarrayExpr<E1, E2, Op>, std::plus<ChooseType<N, ChooseType<typename E1::value_type, typename E2::value_type>>>>
operator+(N const & num, ValarrayExpr<E1, E2, Op> const & expr) {   // number + expr
    ValarrayExpr<Number<N>, ValarrayExpr<E1, E2, Op>, std::plus<ChooseType<N, ChooseType<typename E1::value_type, typename E2::value_type>>>>
        result(num, expr, std::plus<ChooseType<N, ChooseType<typename E1::value_type, typename E2::value_type>>>());
    return result;
}

/* Operator - */
template <typename N, typename T>
ValarrayExpr<Number<N>, valarray<T>, std::minus<ChooseType<N, T>>> operator-(N const & num, valarray<T> const & arr) {   // number - valarray
    ValarrayExpr<Number<N>, valarray<T>, std::minus<ChooseType<N, T>>> result(num, arr, std::minus<ChooseType<N, T>>());
    return result;
}
template <typename N, typename E1, typename E2, typename Op>
ValarrayExpr<Number<N>, ValarrayExpr<E1, E2, Op>, std::minus<ChooseType<N, ChooseType<typename E1::value_type, typename E2::value_type>>>>
operator-(N const & num, ValarrayExpr<E1, E2, Op> const & expr) {   // number - expr
    ValarrayExpr<Number<N>, ValarrayExpr<E1, E2, Op>, std::minus<ChooseType<N, ChooseType<typename E1::value_type, typename E2::value_type>>>>
        result(num, expr, std::minus<ChooseType<N, ChooseType<typename E1::value_type, typename E2::value_type>>>());
    return result;
}

/* Operator * */
template <typename N, typename T>
ValarrayExpr<Number<N>, valarray<T>, std::multiplies<ChooseType<N, T>>> operator*(N const & num, valarray<T> const & arr) {   // number * valarray
    ValarrayExpr<Number<N>, valarray<T>, std::multiplies<ChooseType<N, T>>> result(num, arr, std::multiplies<ChooseType<N, T>>());
    return result;
}
template <typename N, typename E1, typename E2, typename Op>
ValarrayExpr<Number<N>, ValarrayExpr<E1, E2, Op>, std::multiplies<ChooseType<N, ChooseType<typename E1::value_type, typename E2::value_type>>>>
operator*(N const & num, ValarrayExpr<E1, E2, Op> const & expr) {   // number * expr
    ValarrayExpr<Number<N>, ValarrayExpr<E1, E2, Op>, std::multiplies<ChooseType<N, ChooseType<typename E1::value_type, typename E2::value_type>>>>
        result(num, expr, std::multiplies<ChooseType<N, ChooseType<typename E1::value_type, typename E2::value_type>>>());
    return result;
}

/* Operator / */
template <typename N, typename T>
ValarrayExpr<Number<N>, valarray<T>, std::divides<ChooseType<N, T>>> operator/(N const & num, valarray<T> const & arr) {   // number / valarray
    ValarrayExpr<Number<N>, valarray<T>, std::divides<ChooseType<N, T>>> result(num, arr, std::divides<ChooseType<N, T>>());
    return result;
}
template <typename N, typename E1, typename E2, typename Op>
ValarrayExpr<Number<N>, ValarrayExpr<E1, E2, Op>, std::divides<ChooseType<N, ChooseType<typename E1::value_type, typename E2::value_type>>>>
operator/(N const & num, ValarrayExpr<E1, E2, Op> const & expr) {   // number / expr
    ValarrayExpr<Number<N>, ValarrayExpr<E1, E2, Op>, std::divides<ChooseType<N, ChooseType<typename E1::value_type, typename E2::value_type>>>>
        result(num, expr, std::divides<ChooseType<N, ChooseType<typename E1::value_type, typename E2::value_type>>>());
    return result;
}


/*
 * ValarrayExpr Class
 */

template <typename E1, typename E2, typename Op>
ValarrayExpr<E1, E2, Op>::ValarrayExpr(E1 const & arr1, E2 const & arr2, Op op) : m_arr1{ arr1 }, m_arr2{ arr2 } {
    this->m_size = std::min<uint64_t>(arr1.size(), arr2.size());
    this->op = op;
}

template <typename E1, typename E2, typename Op>
ChooseType<typename E1::value_type, typename E2::value_type> ValarrayExpr<E1, E2, Op>::operator[](uint64_t k) const {
    return this->op(this->m_arr1[k], this->m_arr2[k]);
}

template <typename E1, typename E2, typename Op>
uint64_t ValarrayExpr<E1, E2, Op>::size() const {
    return this->m_size;
}


/* Operator + */
template <typename E1, typename E2, typename Op>
template <typename T>
ValarrayExpr<ValarrayExpr<E1, E2, Op>, valarray<T>, std::plus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>>
ValarrayExpr<E1, E2, Op>::operator+(valarray<T> const & arr) {  // expr + valarray
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, valarray<T>, std::plus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>>
        result(*this, arr, std::plus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>());
    return result;
}
template <typename E1, typename E2, typename Op>
template <typename F1, typename F2, typename FOp>
ValarrayExpr<ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
    std::plus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>>>>
ValarrayExpr<E1, E2, Op>::operator+(ValarrayExpr<F1, F2, FOp> const & expr) {   // expr + expr
    ValarrayExpr <ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
        std::plus <ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>>>>
        result(*this, expr, std::plus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>>>());
    return result;
}
template <typename E1, typename E2, typename Op>
template <typename N>
ValarrayExpr<ValarrayExpr<E1, E2, Op>, Number<N>, std::plus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>>
ValarrayExpr<E1, E2, Op>::operator+(N const & num) {    // expr + number
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, Number<N>, std::plus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>>
        result(*this, num, std::plus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>());
    return result;
}

/* Operator - */
template <typename E1, typename E2, typename Op>
template <typename T>
ValarrayExpr<ValarrayExpr<E1, E2, Op>, valarray<T>, std::minus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>>
ValarrayExpr<E1, E2, Op>::operator-(valarray<T> const & arr) {  // expr - valarray
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, valarray<T>, std::minus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>>
        result(*this, arr, std::minus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>());
    return result;
}
template <typename E1, typename E2, typename Op>
template <typename F1, typename F2, typename FOp>
ValarrayExpr<ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
    std::minus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>> >>
    ValarrayExpr<E1, E2, Op>::operator-(ValarrayExpr<F1, F2, FOp> const & expr) {   // expr - expr
    ValarrayExpr <ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
        std::minus <ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>> >>
        result(*this, expr, std::minus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>>>());
    return result;
}
template <typename E1, typename E2, typename Op>
template <typename N>
ValarrayExpr<ValarrayExpr<E1, E2, Op>, Number<N>, std::minus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>>
ValarrayExpr<E1, E2, Op>::operator-(N const & num) {    // expr - number
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, Number<N>, std::minus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>>
        result(*this, num, std::minus<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>());
    return result;
}

/* Operator * */
template <typename E1, typename E2, typename Op>
template <typename T>
ValarrayExpr<ValarrayExpr<E1, E2, Op>, valarray<T>, std::multiplies<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>>
ValarrayExpr<E1, E2, Op>::operator*(valarray<T> const & arr) {  // expr * valarray
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, valarray<T>, std::multiplies<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>>
        result(*this, arr, std::multiplies<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>());
    return result;
}
template <typename E1, typename E2, typename Op>
template <typename F1, typename F2, typename FOp>
ValarrayExpr<ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
    std::multiplies<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>> >>
    ValarrayExpr<E1, E2, Op>::operator*(ValarrayExpr<F1, F2, FOp> const & expr) {   // expr * expr
    ValarrayExpr <ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
        std::multiplies <ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>> >>
        result(*this, expr, std::multiplies<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>>>());
    return result;
}
template <typename E1, typename E2, typename Op>
template <typename N>
ValarrayExpr<ValarrayExpr<E1, E2, Op>, Number<N>, std::multiplies<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>>
ValarrayExpr<E1, E2, Op>::operator*(N const & num) {    // expr * number
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, Number<N>, std::multiplies<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>>
        result(*this, num, std::multiplies<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>());
    return result;
}

/* Operator / */
template <typename E1, typename E2, typename Op>
template <typename T>
ValarrayExpr<ValarrayExpr<E1, E2, Op>, valarray<T>, std::divides<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>>
ValarrayExpr<E1, E2, Op>::operator/(valarray<T> const & arr) {  // expr / valarray
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, valarray<T>, std::divides<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>>
        result(*this, arr, std::divides<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, T>>());
    return result;
}
template <typename E1, typename E2, typename Op>
template <typename F1, typename F2, typename FOp>
ValarrayExpr<ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
    std::divides<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>> >>
    ValarrayExpr<E1, E2, Op>::operator/(ValarrayExpr<F1, F2, FOp> const & expr) {   // expr / expr
    ValarrayExpr <ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
        std::divides <ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>> >>
        result(*this, expr, std::divides<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, ChooseType<typename F1::value_type, typename F2::value_type>>>());
    return result;
}
template <typename E1, typename E2, typename Op>
template <typename N>
ValarrayExpr<ValarrayExpr<E1, E2, Op>, Number<N>, std::divides<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>>
ValarrayExpr<E1, E2, Op>::operator/(N const & num) {    // expr / number
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, Number<N>, std::divides<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>>
        result(*this, num, std::divides<ChooseType<ChooseType<typename E1::value_type, typename E2::value_type>, N>>());
    return result;
}

/* Unary Operator - */
template <typename E1, typename E2, typename Op>
ValarrayExpr<Number<int>, ValarrayExpr<E1, E2, Op>, std::minus<typename ValarrayExpr<E1, E2, Op>::value_type>>
ValarrayExpr<E1, E2, Op>::operator-() { // - expr
    ValarrayExpr<Number<int>, ValarrayExpr<E1, E2, Op>, std::minus<typename ValarrayExpr<E1, E2, Op>::value_type>>
        result(0, *this, std::minus<typename ValarrayExpr<E1, E2, Op>::value_type>());
    return result;
}

/*
 * Valarray Class
 */
/* Default Constructor - inherited */
template <typename T>
valarray<T>::valarray() : vector<T>() {}

template <typename T>
template <typename T2>
valarray<T>::valarray(valarray<T2> const & arr) : vector<T>(arr.size()) {
    for (uint64_t i = 0; i < arr.size(); i++) {
        (*this)[i] = arr[i];
    }
}

template <typename T>
template <typename E1, typename E2, typename Op>
valarray<T>::valarray(ValarrayExpr<E1, E2, Op> const & arr) : vector<T>(arr.size()) {
    for (uint64_t i = 0; i < arr.size(); i++) {
        (*this)[i] = arr[i];
    }
}

/* Operator = */
template <typename T>
template <typename T2>
valarray<ChooseType<T, T2>> & valarray<T>::operator=(valarray<T2> const & arr) {
    for (uint64_t i = 0; i < std::min<uint64_t>(this->size(), arr.size()); i++) {
        (*this)[i] = arr[i];
    }
    return *this;
}
template <typename T>
template <typename E1, typename E2, typename Op>
valarray<ChooseType<typename E1::value_type, typename E2::value_type>> & valarray<T>::operator=(ValarrayExpr<E1, E2, Op> const & arr) {
    for (uint64_t i = 0; i < std::min<uint64_t>(this->size(), arr.size()); i++) {
        (*this)[i] = arr[i];
    }
    return *this;
}
template <typename T>
template <typename N>
valarray<ChooseType<T, N>> & valarray<T>::operator=(N const & num) {
    Number<N> num_wrap{ num };
    for (uint64_t i = 0; i < std::min<uint64_t>(this->size(), num_wrap.size()); i++) {
        (*this)[i] = num_wrap[i];
    }
}

/* Operator += */
template <typename T>
template <typename T2>
valarray<ChooseType<T, T2>> & valarray<T>::operator+=(valarray<T2> const & arr) {    // valarray += valarray
    *this = *this + arr;
    return *this;
}
template <typename T>
template <typename E1, typename E2, typename Op>
valarray<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>> & valarray<T>::operator+=(ValarrayExpr<E1, E2, Op> const & expr) {    // valarray += expr
    *this = *this + expr;
    return *this;
}
template <typename T>
template <typename N>
valarray<ChooseType<T, N>> & valarray<T>::operator+=(N const & num) {
    *this = *this + num;
    return *this;
}

/* Operator -= */
template <typename T>
template <typename T2>
valarray<ChooseType<T, T2>> & valarray<T>::operator-=(valarray<T2> const & arr) {    // valarray -= valarray
    *this = *this - arr;
    return *this;
}
template <typename T>
template <typename E1, typename E2, typename Op>
valarray<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>> & valarray<T>::operator-=(ValarrayExpr<E1, E2, Op> const & expr) {    // valarray -= expr
    *this = *this - expr;
    return *this;
}
template <typename T>
template <typename N>
valarray<ChooseType<T, N>> & valarray<T>::operator-=(N const & num) {
    *this = *this - num;
    return *this;
}

/* Operator *= */
template <typename T>
template <typename T2>
valarray<ChooseType<T, T2>> & valarray<T>::operator*=(valarray<T2> const & arr) {    // valarray *= valarray
    *this = *this * arr;
    return *this;
}
template <typename T>
template <typename E1, typename E2, typename Op>
valarray<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>> & valarray<T>::operator*=(ValarrayExpr<E1, E2, Op> const & expr) {    // valarray *= expr
    *this = *this * expr;
    return *this;
}
template <typename T>
template <typename N>
valarray<ChooseType<T, N>> & valarray<T>::operator*=(N const & num) {
    *this = *this * num;
    return *this;
}

/* Operator /= */
template <typename T>
template <typename T2>
valarray<ChooseType<T, T2>> & valarray<T>::operator/=(valarray<T2> const & arr) {    // valarray /= valarray
    *this = *this / arr;
    return *this;
}
template <typename T>
template <typename E1, typename E2, typename Op>
valarray<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>> & valarray<T>::operator/=(ValarrayExpr<E1, E2, Op> const & expr) {    // valarray /= expr
    *this = *this / expr;
    return *this;
}
template <typename T>
template <typename N>
valarray<ChooseType<T, N>> & valarray<T>::operator/=(N const & num) {
    *this = *this / num;
    return *this;
}

/* Operator + */
template <typename T>
template <typename T2>
ValarrayExpr<valarray<T>, valarray<T2>, std::plus<ChooseType<T, T2>>> valarray<T>::operator+(valarray<T2> const & rhs) {    // valarray + valarray
    ValarrayExpr<valarray<T>, valarray<T2>, std::plus<ChooseType<T, T2>>> result (*this, rhs, std::plus<ChooseType<T, T2>>());
    return result;
}
template <typename T>
template <typename E1, typename E2, typename Op>
ValarrayExpr<valarray<T>, ValarrayExpr<E1, E2, Op>, std::plus<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>>
valarray<T>::operator+(ValarrayExpr<E1, E2, Op> const & arr) {  // valarray + expr
    ValarrayExpr <valarray<T>, ValarrayExpr<E1, E2, Op>, std::plus <ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>>
        result(*this, arr, std::plus<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>());
    return result;
}
template <typename T>
template <typename N>
ValarrayExpr<valarray<T>, Number<N>, std::plus<ChooseType<T, N>>> valarray<T>::operator+(N const & num) {   // valarray + number
    ValarrayExpr<valarray<T>, Number<N>, std::plus<ChooseType<T, N>>> result(*this, num, std::plus<ChooseType<T, N>>());
    return result;
}

/* Operator - */
template <typename T>
template <typename T2>
ValarrayExpr<valarray<T>, valarray<T2>, std::minus<ChooseType<T, T2>>> valarray<T>::operator-(valarray<T2> const & rhs) {    // valarray - valarray
    ValarrayExpr<valarray<T>, valarray<T2>, std::minus<ChooseType<T, T2>>> result(*this, rhs, std::minus<ChooseType<T, T2>>());
    return result;
}
template <typename T>
template <typename E1, typename E2, typename Op>
ValarrayExpr<valarray<T>, ValarrayExpr<E1, E2, Op>, std::minus<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>>
valarray<T>::operator-(ValarrayExpr<E1, E2, Op> const & arr) {  // valarray - expr
    ValarrayExpr <valarray<T>, ValarrayExpr<E1, E2, Op>, std::minus <ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>>
        result(*this, arr, std::minus<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>());
    return result;
}
template <typename T>
template <typename N>
ValarrayExpr<valarray<T>, Number<N>, std::minus<ChooseType<T, N>>> valarray<T>::operator-(N const & num) {   // valarray - number
    ValarrayExpr<valarray<T>, Number<N>, std::minus<ChooseType<T, N>>> result(*this, num, std::minus<ChooseType<T, N>>());
    return result;
}

/* Operator * */
template <typename T>
template <typename T2>
ValarrayExpr<valarray<T>, valarray<T2>, std::multiplies<ChooseType<T, T2>>> valarray<T>::operator*(valarray<T2> const & rhs) {    // valarray * valarray
    ValarrayExpr<valarray<T>, valarray<T2>, std::multiplies<ChooseType<T, T2>>> result(*this, rhs, std::multiplies<ChooseType<T, T2>>());
    return result;
}
template <typename T>
template <typename E1, typename E2, typename Op>
ValarrayExpr<valarray<T>, ValarrayExpr<E1, E2, Op>, std::multiplies<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>>
valarray<T>::operator*(ValarrayExpr<E1, E2, Op> const & arr) {  // valarray * expr
    ValarrayExpr <valarray<T>, ValarrayExpr<E1, E2, Op>, std::multiplies <ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>>
        result(*this, arr, std::multiplies<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>());
    return result;
}
template <typename T>
template <typename N>
ValarrayExpr<valarray<T>, Number<N>, std::multiplies<ChooseType<T, N>>> valarray<T>::operator*(N const & num) {   // valarray * number
    ValarrayExpr<valarray<T>, Number<N>, std::multiplies<ChooseType<T, N>>> result(*this, num, std::multiplies<ChooseType<T, N>>());
    return result;
}

/* Operator / */
template <typename T>
template <typename T2>
ValarrayExpr<valarray<T>, valarray<T2>, std::divides<ChooseType<T, T2>>> valarray<T>::operator/(valarray<T2> const & rhs) {    // valarray / valarray
    ValarrayExpr<valarray<T>, valarray<T2>, std::divides<ChooseType<T, T2>>> result(*this, rhs, std::divides<ChooseType<T, T2>>());
    return result;
}
template <typename T>
template <typename E1, typename E2, typename Op>
ValarrayExpr<valarray<T>, ValarrayExpr<E1, E2, Op>, std::divides<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>>
valarray<T>::operator/(ValarrayExpr<E1, E2, Op> const & arr) {  // valarray / expr
    ValarrayExpr <valarray<T>, ValarrayExpr<E1, E2, Op>, std::divides <ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>>
        result(*this, arr, std::divides<ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>());
    return result;
}
template <typename T>
template <typename N>
ValarrayExpr<valarray<T>, Number<N>, std::divides<ChooseType<T, N>>> valarray<T>::operator/(N const & num) {   // valarray / number
    ValarrayExpr<valarray<T>, Number<N>, std::divides<ChooseType<T, N>>> result(*this, num, std::divides<ChooseType<T, N>>());
    return result;
}

/* Unary Operator - */
template <typename T>
ValarrayExpr<Number<int>, valarray<T>, std::minus<T>> valarray<T>::operator-() {
    ValarrayExpr<Number<int>, valarray<T>, std::minus<T>> result(0, *this, std::minus<T>());
    return result;
}


#endif /* _Valarray_h */

