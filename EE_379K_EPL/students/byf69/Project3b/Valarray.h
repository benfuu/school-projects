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
#include <cstdint>
#include <algorithm>
#include <complex>
#include <functional>
#include <cmath>
#include <type_traits>
#include <utility>
//#include <vector>
#include "Vector.h"

using std::cout;
using std::endl;
using std::complex;
using namespace std::rel_ops;

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


/* Sqrt Function */
template <typename T>
struct Sqrt {
    using argument_type = T;
    using result_type = double;
    double operator()(T const &) const;
};

template <typename T>
struct Sqrt<std::complex<T>> {
    T m_val;
    using argument_type = std::complex<T>;
    using result_type = complex<double>;
    Sqrt(T val) : m_val{ val } {};
    complex<double> operator()(std::complex<T> const &) const;
};

/* VoidExpr - Helps for ValarrayExpr that only use Unary Ops */
class VoidExpr {
    uint64_t m_size = std::numeric_limits<uint64_t>::max();
public:
    using value_type = int;
    uint64_t size() const { return this->m_size; }
    int operator[](uint64_t k) const { return 0; }
};

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
    E1Type const m_arr1;
    E2Type const m_arr2;
    uint64_t m_size;
    Op op;
public:
    using value_type = ChooseType<typename E1::value_type, typename E2::value_type>;
    ValarrayExpr(E1 const &, E2 const &, Op);
    ChooseType<typename E1::value_type, typename E2::value_type> operator[](uint64_t) const;
    template <typename F1, typename F2, typename FOp>
    bool operator==(ValarrayExpr<F1, F2, FOp> const &) const;
    uint64_t size() const;
    /* Iterator */
    class const_iterator : public std::iterator<std::random_access_iterator_tag, ChooseType<typename E1::value_type, typename E2::value_type>> {
    private:
        E1Type const m_arr1;
        E2Type const m_arr2;
        Op op;
        uint64_t m_index;
    public:
        using difference_type = typename std::iterator<std::random_access_iterator_tag, value_type>::difference_type;
        /* Constructors */
        const_iterator();
        const_iterator(const_iterator const &);
        const_iterator(E1Type const &, E2Type const &, Op, uint64_t);
        void operator=(const_iterator const &);
        /* Comparators */
        bool operator==(const_iterator const &);
        bool operator!=(const_iterator const &);
        bool operator<(const_iterator const &);
        /* Evaluate Operators */
        typename ValarrayExpr<E1, E2, Op>::value_type operator*() const;
        typename ValarrayExpr<E1, E2, Op>::value_type operator[](uint64_t) const;
        /* Arithemtic Operators: +, -, ++, -- */
        const_iterator operator+(difference_type);
        template <typename N>
        friend const_iterator operator+(N const &, const_iterator const &);
        const_iterator operator-(difference_type);
        difference_type operator-(const_iterator const &);
        const_iterator operator++();
        const_iterator operator++(int);
        const_iterator operator--();
        const_iterator operator--(int);
        /* Compound Arithmetic Operators: +=, -= */
        const_iterator operator+=(difference_type);
        const_iterator operator-=(difference_type);
    };
    const_iterator begin() const;
    const_iterator end() const;
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
    /* Accumulate - Applies function to all elements. */
    template <typename Funct>
    ChooseType<typename E1::value_type, typename E2::value_type> accumulate(Funct);
    /* Sum - Adds up all elements in valarray. */
    ChooseType<typename E1::value_type, typename E2::value_type> sum();
    /* Apply - Applies function to all elements. */
    template <typename Funct>
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, VoidExpr, Funct> apply(Funct);
    /* Sqrt */
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, VoidExpr, Sqrt<ChooseType<typename E1::value_type, typename E2::value_type>>> sqrt();
};

template <typename E1, typename Op>
class ValarrayExpr<E1, VoidExpr, Op> {
private:
    using E1Type = ChooseRef<E1>;
    using E2Type = ChooseRef<VoidExpr>;
    E1Type const m_arr1;
    E2Type const m_arr2;
    uint64_t m_size;
    Op op;
public:
    using value_type = typename E1::value_type;
    ValarrayExpr(E1 const &, VoidExpr const &, Op);
    typename E1::value_type operator[](uint64_t) const;
    template <typename F1, typename F2, typename FOp>
    bool operator==(ValarrayExpr<F1, F2, FOp> const &) const;
    uint64_t size() const;
    /* Iterator */
    class const_iterator : public std::iterator<std::random_access_iterator_tag, typename E1::value_type> {
    private:
        E1Type const m_arr1;
        E2Type const m_arr2;
        Op op;
        uint64_t m_index;
    public:
        using difference_type = typename std::iterator<std::random_access_iterator_tag, value_type>::difference_type;
        /* Constructors */
        const_iterator();
        const_iterator(const_iterator const &);
        const_iterator(E1Type const &, VoidExpr const &, Op, uint64_t);
        void operator=(const_iterator const &);
        /* Comparators */
        bool operator==(const_iterator const &);
        bool operator!=(const_iterator const &);
        bool operator<(const_iterator const &);
        /* Evaluate Operators */
        typename ValarrayExpr<E1, VoidExpr, Op>::value_type operator*() const;
        typename ValarrayExpr<E1, VoidExpr, Op>::value_type operator[](uint64_t) const;
        /* Arithemtic Operators: +, -, ++, -- */
        const_iterator operator+(difference_type);
        template <typename N>
        friend const_iterator operator+(N const &, const_iterator const &);
        const_iterator operator-(difference_type);
        difference_type operator-(const_iterator const &);
        const_iterator operator++();
        const_iterator operator++(int);
        const_iterator operator--();
        const_iterator operator--(int);
        /* Compound Arithmetic Operators: +=, -= */
        const_iterator operator+=(difference_type);
        const_iterator operator-=(difference_type);
    };
    const_iterator begin() const;
    const_iterator end() const;
    /* Operator + */
    template <typename T>
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, valarray<T>,
        std::plus<ChooseType<typename E1::value_type, T>>>
        operator+(valarray<T> const &); // expr + valarray
    template <typename F1, typename F2, typename FOp>
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, ValarrayExpr<F1, F2, FOp>,
        std::plus<ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>> >>
        operator+(ValarrayExpr<F1, F2, FOp> const &);   // expr + expr
    template <typename N>
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, Number<N>,
        std::plus<ChooseType<typename E1::value_type, N>>>
        operator+(N const &);   // expr + number
    /* Operator - */
    template <typename T>
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, valarray<T>,
        std::minus<ChooseType<typename E1::value_type, T>>>
        operator-(valarray<T> const &); // expr - valarray
    template <typename F1, typename F2, typename FOp>
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, ValarrayExpr<F1, F2, FOp>,
        std::minus<ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>> >>
        operator-(ValarrayExpr<F1, F2, FOp> const &);   // expr - expr
    template <typename N>
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, Number<N>,
        std::minus<ChooseType<typename E1::value_type, N>>>
        operator-(N const &);   // expr - number
    /* Operator * */
    template <typename T>
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, valarray<T>,
        std::multiplies<ChooseType<typename E1::value_type, T>>>
        operator*(valarray<T> const &); // expr * valarray
    template <typename F1, typename F2, typename FOp>
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, ValarrayExpr<F1, F2, FOp>,
        std::multiplies<ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>> >>
        operator*(ValarrayExpr<F1, F2, FOp> const &);   // expr * expr
    template <typename N>
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, Number<N>,
        std::multiplies<ChooseType<typename E1::value_type, N>>>
        operator*(N const &);   // expr * number
    /* Operator / */
    template <typename T>
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, valarray<T>,
        std::divides<ChooseType<typename E1::value_type, T>>>
        operator/(valarray<T> const &); // expr / valarray
    template <typename F1, typename F2, typename FOp>
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, ValarrayExpr<F1, F2, FOp>,
        std::divides<ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>> >>
        operator/(ValarrayExpr<F1, F2, FOp> const &);   // expr / expr
    template <typename N>
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, Number<N>,
        std::divides<ChooseType<typename E1::value_type, N>>>
        operator/(N const &);   // expr / number
    /* Unary Operator - */
    ValarrayExpr<Number<int>, ValarrayExpr<E1, VoidExpr, Op>, std::minus<typename ValarrayExpr<E1, VoidExpr, Op>::value_type>> operator-();   // - expr
    /* Accumulate - Applies function to all elements. */
    template <typename Funct>
    typename E1::value_type accumulate(Funct);
    /* Sum - Adds up all elements in valarray. */
    typename E1::value_type sum();
    /* Apply - Applies function to all elements. */
    template <typename Funct>
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, VoidExpr, Funct> apply(Funct);
    /* Sqrt */
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, VoidExpr, Sqrt<typename E1::value_type>> sqrt();
};

//template <typename E1, typename E2, typename Op>
//class ValarrayExpr : public ValarrayExpr<E1, E2, Op> {
//protected:
//    
//public:
//    //using value_type = typename ValarrayExpr<E1, E2, Op>::value_type;
//    using ValarrayExpr<E1, E2, Op>::ValarrayExpr;
//    ValarrayExpr(E1 const &, E2 const &, Op);
//    virtual ChooseType<typename E1::value_type, typename E2::value_type> operator[](uint64_t) const;
//    /* Operators */
//    template <typename F1, typename F2, typename FOp>
//    ValarrayExpr<ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>, std::plus<>> operator+(ValarrayExpr<F1, F2, FOp> const &);
//};

//template <typename E1, typename E2, typename Op>
//class ValarrayExprUnary : public ValarrayExpr<E1, E2, Op> {
//protected:
//public:
//    //using value_type = typename ValarrayExpr<E1, E2, Op>::value_type;
//    using ValarrayExpr<E1, E2, Op>::ValarrayExpr;
//    ValarrayExprUnary(E1 const &, E2 const &, Op);
//    virtual ChooseType<typename E1::value_type, typename E2::value_type> operator[](uint64_t) const;
//};




/* Valarray Class */
template <typename T>
class valarray : public vector<T> {
public:
    using vector<T>::vector;    // inherit vector constructors
    template <typename E1, typename E2, typename Op>
    friend class ValarrayExpr;
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
    /* Sum - Adds up all elements in valarray. */
    T sum();
    /* Accumulate - Applies function to all elements. */
    template <typename Funct>
    T accumulate(Funct);
    /* Apply - Applies function to all elements. */
    template <typename Funct>
    ValarrayExpr<valarray<T>, VoidExpr, Funct> apply(Funct);
    /* Sqrt */
    ValarrayExpr<valarray<T>, VoidExpr, Sqrt<T>> sqrt();
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




/* Function Definitions */

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
ValarrayExpr<E1, E2, Op>::ValarrayExpr(E1 const & arr1, E2 const & arr2, Op op)
    : m_arr1{ arr1 }, m_arr2{ arr2 }, op{ op } {
    this->m_size = std::min<uint64_t>(arr1.size(), arr2.size());
    //this->op = op;
}

template <typename E1, typename E2, typename Op>
ChooseType<typename E1::value_type, typename E2::value_type>
ValarrayExpr<E1, E2, Op>::operator[](uint64_t k) const {
    return this->op(this->m_arr1[k], this->m_arr2[k]);
}

//template <typename E1, typename E2, typename Op>
//ChooseType<typename E1::value_type, typename E2::value_type> ValarrayExpr<E1, E2, Op>::apply_op(uint64_t k) const {
//    return this->op(this->m_arr1[k], this->m_arr2[k]);
//}

template <typename E1, typename E2, typename Op>
template <typename F1, typename F2, typename FOp>
bool ValarrayExpr<E1, E2, Op>::operator==(ValarrayExpr<F1, F2, FOp> const & expr) const {
    return this->m_arr1 == expr.m_arr1 && this->m_arr2 == expr.m_arr2;
}

template <typename E1, typename E2, typename Op>
uint64_t ValarrayExpr<E1, E2, Op>::size() const {
    return this->m_size;
}


/* Iterator Functions */
template <typename E1, typename E2, typename Op>
typename ValarrayExpr<E1, E2, Op>::const_iterator ValarrayExpr<E1, E2, Op>::begin() const {
    return ValarrayExpr<E1, E2, Op>::const_iterator(this->m_arr1, this->m_arr2, this->op, 0);
}

template <typename E1, typename E2, typename Op>
typename ValarrayExpr<E1, E2, Op>::const_iterator ValarrayExpr<E1, E2, Op>::end() const {
    return ValarrayExpr<E1, E2, Op>::const_iterator(this->m_arr1, this->m_arr2, this->op, this->size());
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
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
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
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
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
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
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
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, ValarrayExpr<F1, F2, FOp>,
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

/* Accumulate - Applies function to all elements. */
template <typename E1, typename E2, typename Op>
template <typename Funct>
ChooseType<typename E1::value_type, typename E2::value_type> ValarrayExpr<E1, E2, Op>::accumulate(Funct funct) {
    if (this->size() == 0) { return (ChooseType<typename E1::value_type, typename E2::value_type>)0; }
    if (this->size() == 1) { return (*this)[0]; }
    ChooseType<typename E1::value_type, typename E2::value_type> result = (*this)[0];
    for (uint64_t i = 1; i < this->size(); i++) {
        result = funct(result, (*this)[i]);
    }
    return result;
}

/* Sum - Adds up all elements in valarray. */
template <typename E1, typename E2, typename Op>
ChooseType<typename E1::value_type, typename E2::value_type> ValarrayExpr<E1, E2, Op>::sum() {
    return this->accumulate(std::plus<ChooseType<typename E1::value_type, typename E2::value_type>>{});
}

/* Apply - Applies function to all elements. */
template <typename E1, typename E2, typename Op>
template <typename Funct>
ValarrayExpr<ValarrayExpr<E1, E2, Op>, VoidExpr, Funct> ValarrayExpr<E1, E2, Op>::apply(Funct funct) {
    ValarrayExpr<ValarrayExpr<E1, E2, Op>, VoidExpr, Funct> result(*this, VoidExpr{}, funct);
    return result;
}

/* Sqrt */
template <typename E1, typename E2, typename Op>
ValarrayExpr<ValarrayExpr<E1, E2, Op>, VoidExpr, Sqrt<ChooseType<typename E1::value_type, typename E2::value_type>>> ValarrayExpr<E1, E2, Op>::sqrt() {
    return this->apply(Sqrt<ChooseType<typename E1::value_type, typename E2::value_type>>{});
}



/*
 * ValarrayExpr Unary Specialization
 */

template <typename E1, typename Op>
ValarrayExpr<E1, VoidExpr, Op>::ValarrayExpr(E1 const & arr1, VoidExpr const & voidexpr, Op op)
    : m_arr1{ arr1 }, m_arr2{ voidexpr }, op{ op } {
    this->m_size = arr1.size();
    //this->op = op;
}

template <typename E1, typename Op>
typename E1::value_type ValarrayExpr<E1, VoidExpr, Op>::operator[](uint64_t k) const {
    return this->op(this->m_arr1[k]);
}

template <typename E1, typename Op>
template <typename F1, typename F2, typename FOp>
bool ValarrayExpr<E1, VoidExpr, Op>::operator==(ValarrayExpr<F1, F2, FOp> const & expr) const {
    return this->m_arr1 == expr.m_arr1 && this->m_arr2 == expr.m_arr2;
}

template <typename E1, typename Op>
uint64_t ValarrayExpr<E1, VoidExpr, Op>::size() const {
    return this->m_size;
}

/* Iterator Functions */
template <typename E1, typename Op>
typename ValarrayExpr<E1, VoidExpr, Op>::const_iterator ValarrayExpr<E1, VoidExpr, Op>::begin() const {
    return ValarrayExpr<E1, VoidExpr, Op>::const_iterator(this->m_arr1, this->m_arr2, this->op, 0);
}

template <typename E1, typename Op>
typename ValarrayExpr<E1, VoidExpr, Op>::const_iterator ValarrayExpr<E1, VoidExpr, Op>::end() const {
    return ValarrayExpr<E1, VoidExpr, Op>::const_iterator(this->m_arr1, this->m_arr2, this->op, this->size());
}

/* Operator + */
template <typename E1, typename Op>
template <typename T>
ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, valarray<T>, std::plus<ChooseType<typename E1::value_type, T>>>
ValarrayExpr<E1, VoidExpr, Op>::operator+(valarray<T> const & arr) {  // expr + valarray
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, valarray<T>, std::plus<ChooseType<typename E1::value_type, T>>>
        result(*this, arr, std::plus<ChooseType<typename E1::value_type, T>>());
    return result;
}
template <typename E1, typename Op>
template <typename F1, typename F2, typename FOp>
ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, ValarrayExpr<F1, F2, FOp>,
    std::plus<ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>>>>
    ValarrayExpr<E1, VoidExpr, Op>::operator+(ValarrayExpr<F1, F2, FOp> const & expr) {   // expr + expr
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, ValarrayExpr<F1, F2, FOp>,
        std::plus <ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>>>>
        result(*this, expr, std::plus<ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>>>());
    return result;
}
template <typename E1, typename Op>
template <typename N>
ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, Number<N>, std::plus<ChooseType<typename E1::value_type, N>>>
ValarrayExpr<E1, VoidExpr, Op>::operator+(N const & num) {    // expr + number
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, Number<N>, std::plus<ChooseType<typename E1::value_type, N>>>
        result(*this, num, std::plus<ChooseType<typename E1::value_type, N>>());
    return result;
}

/* Operator - */
template <typename E1, typename Op>
template <typename T>
ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, valarray<T>, std::minus<ChooseType<typename E1::value_type, T>>>
ValarrayExpr<E1, VoidExpr, Op>::operator-(valarray<T> const & arr) {  // expr - valarray
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, valarray<T>, std::minus<ChooseType<typename E1::value_type, T>>>
        result(*this, arr, std::minus<ChooseType<typename E1::value_type, T>>());
    return result;
}
template <typename E1, typename Op>
template <typename F1, typename F2, typename FOp>
ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, ValarrayExpr<F1, F2, FOp>,
    std::minus<ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>> >>
    ValarrayExpr<E1, VoidExpr, Op>::operator-(ValarrayExpr<F1, F2, FOp> const & expr) {   // expr - expr
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, ValarrayExpr<F1, F2, FOp>,
        std::minus <ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>> >>
        result(*this, expr, std::minus<ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>>>());
    return result;
}
template <typename E1, typename Op>
template <typename N>
ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, Number<N>, std::minus<ChooseType<typename E1::value_type, N>>>
ValarrayExpr<E1, VoidExpr, Op>::operator-(N const & num) {    // expr - number
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, Number<N>, std::minus<ChooseType<typename E1::value_type, N>>>
        result(*this, num, std::minus<ChooseType<typename E1::value_type, N>>());
    return result;
}

/* Operator * */
template <typename E1, typename Op>
template <typename T>
ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, valarray<T>, std::multiplies<ChooseType<typename E1::value_type, T>>>
ValarrayExpr<E1, VoidExpr, Op>::operator*(valarray<T> const & arr) {  // expr * valarray
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, valarray<T>, std::multiplies<ChooseType<typename E1::value_type, T>>>
        result(*this, arr, std::multiplies<ChooseType<typename E1::value_type, T>>());
    return result;
}
template <typename E1, typename Op>
template <typename F1, typename F2, typename FOp>
ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, ValarrayExpr<F1, F2, FOp>,
    std::multiplies<ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>> >>
    ValarrayExpr<E1, VoidExpr, Op>::operator*(ValarrayExpr<F1, F2, FOp> const & expr) {   // expr * expr
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, ValarrayExpr<F1, F2, FOp>,
        std::multiplies <ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>> >>
        result(*this, expr, std::multiplies<ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>>>());
    return result;
}
template <typename E1, typename Op>
template <typename N>
ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, Number<N>, std::multiplies<ChooseType<typename E1::value_type, N>>>
ValarrayExpr<E1, VoidExpr, Op>::operator*(N const & num) {    // expr * number
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, Number<N>, std::multiplies<ChooseType<typename E1::value_type, N>>>
        result(*this, num, std::multiplies<ChooseType<typename E1::value_type, N>>());
    return result;
}

/* Operator / */
template <typename E1, typename Op>
template <typename T>
ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, valarray<T>, std::divides<ChooseType<typename E1::value_type, T>>>
ValarrayExpr<E1, VoidExpr, Op>::operator/(valarray<T> const & arr) {  // expr / valarray
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, valarray<T>, std::divides<ChooseType<typename E1::value_type, T>>>
        result(*this, arr, std::divides<ChooseType<typename E1::value_type, T>>());
    return result;
}
template <typename E1, typename Op>
template <typename F1, typename F2, typename FOp>
ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, ValarrayExpr<F1, F2, FOp>,
    std::divides<ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>> >>
    ValarrayExpr<E1, VoidExpr, Op>::operator/(ValarrayExpr<F1, F2, FOp> const & expr) {   // expr / expr
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, ValarrayExpr<F1, F2, FOp>,
        std::divides <ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>> >>
        result(*this, expr, std::divides<ChooseType<typename E1::value_type, ChooseType<typename F1::value_type, typename F2::value_type>>>());
    return result;
}
template <typename E1, typename Op>
template <typename N>
ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, Number<N>, std::divides<ChooseType<typename E1::value_type, N>>>
ValarrayExpr<E1, VoidExpr, Op>::operator/(N const & num) {    // expr / number
    ValarrayExpr<ValarrayExpr<E1, VoidExpr, Op>, Number<N>, std::divides<ChooseType<typename E1::value_type, N>>>
        result(*this, num, std::divides<ChooseType<typename E1::value_type, N>>());
    return result;
}

/* Unary Operator - */
template <typename E1, typename Op>
ValarrayExpr<Number<int>, ValarrayExpr<E1, VoidExpr, Op>, std::minus<typename ValarrayExpr<E1, VoidExpr, Op>::value_type>>
ValarrayExpr<E1, VoidExpr, Op>::operator-() { // - expr
    ValarrayExpr<Number<int>, ValarrayExpr<E1, VoidExpr, Op>, std::minus<typename ValarrayExpr<E1, VoidExpr, Op>::value_type>>
        result(0, *this, std::minus<typename ValarrayExpr<E1, VoidExpr, Op>::value_type>());
    return result;
}


/*
 * ValarrayExpr Unary Class - Subclass of ValarrayExpr
 */
//template <typename E1, typename E2, typename Op>
//ValarrayExprUnary<E1, E2, Op>::ValarrayExprUnary(E1 const & arr1, E2 const & arr2, Op op) : ValarrayExpr<E1, E2, Op>(arr1, arr2, op) {}
//template <typename E1, typename E2, typename Op>
//ChooseType<typename E1::value_type, typename E2::value_type> ValarrayExprUnary<E1, E2, Op>::operator[](uint64_t k) const {
//    return this->op(this->m_arr1[k]);
//}

/*
 * ValarrayExpr::const_iterator Class
 */

/* Constructors */
template <typename E1, typename E2, typename Op>
ValarrayExpr<E1, E2, Op>::const_iterator::const_iterator() {
    
}

template <typename E1, typename E2, typename Op>
ValarrayExpr<E1, E2, Op>::const_iterator::const_iterator(const_iterator const & it) 
    : m_arr1{ it.m_arr1 }, m_arr2{ it.m_arr2 }, op{ it.op } {
    this->m_index = it.m_index;
}

template <typename E1, typename E2, typename Op>
ValarrayExpr<E1, E2, Op>::const_iterator::const_iterator(E1Type const & arr1, E2Type const & arr2, Op op, uint64_t index)
    : m_arr1{ arr1 }, m_arr2{ arr2 }, op{ op } {
    this->m_index = index;
}

template <typename E1, typename E2, typename Op>
void ValarrayExpr<E1, E2, Op>::const_iterator::operator=(const_iterator const & rhs) {
    this->m_arr1 = rhs.m_arr1;
    this->m_arr2 = rhs.m_arr2;
    this->op = rhs.op;
    this->m_index = rhs.m_index;
}

/* Comparators */
template <typename E1, typename E2, typename Op>
bool ValarrayExpr<E1, E2, Op>::const_iterator::operator==(const_iterator const & rhs) {
    return (this->m_arr1 == rhs.m_arr1) && (this->m_arr2 == rhs.m_arr2) && (this->m_index == rhs.m_index);
}

template <typename E1, typename E2, typename Op>
bool ValarrayExpr<E1, E2, Op>::const_iterator::operator!=(const_iterator const & rhs) {
    return !(*this == rhs);
}

template <typename E1, typename E2, typename Op>
bool ValarrayExpr<E1, E2, Op>::const_iterator::operator<(const_iterator const & rhs) {
    return this->m_index < rhs.m_index;
}

/* Evaluate Operators */
template <typename E1, typename E2, typename Op>
typename ValarrayExpr<E1, E2, Op>::value_type ValarrayExpr<E1, E2, Op>::const_iterator::operator*() const {
    return (*this)[this->m_index];
}

template <typename E1, typename E2, typename Op>
typename ValarrayExpr<E1, E2, Op>::value_type ValarrayExpr<E1, E2, Op>::const_iterator::operator[](uint64_t k) const {
    return this->op(this->m_arr1[k], this->m_arr2[k]);
}

/* Arithemtic Operators: +, -, ++, -- */
template <typename E1, typename E2, typename Op>
typename ValarrayExpr<E1, E2, Op>::const_iterator ValarrayExpr<E1, E2, Op>::const_iterator::operator+(difference_type k) {
    return const_iterator(this->m_arr1, this->m_arr2, this->op, this->m_index + k);
}

template <typename E1, typename E2, typename Op, typename N>
typename ValarrayExpr<E1, E2, Op>::const_iterator operator+(N const & k, typename ValarrayExpr<E1, E2, Op>::const_iterator const & it) {
    return ValarrayExpr<E1, E2, Op>::const_iterator(it.m_arr1, it.m_arr2, it.op, it.index + k);
}

template <typename E1, typename E2, typename Op>
typename ValarrayExpr<E1, E2, Op>::const_iterator ValarrayExpr<E1, E2, Op>::const_iterator::operator-(difference_type k) {
    return const_iterator(this->m_arr1, this->m_arr2, this->op, this->m_index - k);
}

template <typename E1, typename E2, typename Op>
typename ValarrayExpr<E1, E2, Op>::const_iterator::difference_type ValarrayExpr<E1, E2, Op>::const_iterator::operator-(const_iterator const & rhs) {
    return this->m_index - rhs.m_index;
}

template <typename E1, typename E2, typename Op>
typename ValarrayExpr<E1, E2, Op>::const_iterator ValarrayExpr<E1, E2, Op>::const_iterator::operator++() {
    ++this->m_index;
    return *this;
}

template <typename E1, typename E2, typename Op>
typename ValarrayExpr<E1, E2, Op>::const_iterator ValarrayExpr<E1, E2, Op>::const_iterator::operator++(int) {
    const_iterator it(*this);
    ++(*this);
    return it;
}

template <typename E1, typename E2, typename Op>
typename ValarrayExpr<E1, E2, Op>::const_iterator ValarrayExpr<E1, E2, Op>::const_iterator::operator--() {
    --this->m_index;
    return *this;
}

template <typename E1, typename E2, typename Op>
typename ValarrayExpr<E1, E2, Op>::const_iterator ValarrayExpr<E1, E2, Op>::const_iterator::operator--(int) {
    const_iterator it(*this);
    --(*this);
    return it;
}

/* Compound Arithmetic Operators: +=, -= */
template <typename E1, typename E2, typename Op>
typename ValarrayExpr<E1, E2, Op>::const_iterator ValarrayExpr<E1, E2, Op>::const_iterator::operator+=(difference_type k) {
    return *this + k;
}

template <typename E1, typename E2, typename Op>
typename ValarrayExpr<E1, E2, Op>::const_iterator ValarrayExpr<E1, E2, Op>::const_iterator::operator-=(difference_type k) {
    return *this - k;
}


/*
* ValarrayExpr::const_iterator Class - Specialized for Unary Case
*/

/* Constructors */
template <typename E1, typename Op>
ValarrayExpr<E1, VoidExpr, Op>::const_iterator::const_iterator() {

}

template <typename E1, typename Op>
ValarrayExpr<E1, VoidExpr, Op>::const_iterator::const_iterator(const_iterator const & it)
    : m_arr1{ it.m_arr1 }, m_arr2{ it.m_arr2 }, op{ it.op } {
    this->m_index = it.m_index;
}

template <typename E1, typename Op>
ValarrayExpr<E1, VoidExpr, Op>::const_iterator::const_iterator(E1Type const & arr1, VoidExpr const & arr2, Op op, uint64_t index)
    : m_arr1{ arr1 }, m_arr2{ arr2 }, op{ op } {
    this->m_index = index;
}

template <typename E1, typename Op>
void ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator=(const_iterator const & rhs) {
    this->m_arr1 = rhs.m_arr1;
    this->m_arr2 = rhs.m_arr2;
    this->op = rhs.op;
    this->m_index = rhs.m_index;
}

/* Comparators */
template <typename E1, typename Op>
bool ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator==(const_iterator const & rhs) {
    return (this->m_arr1 == rhs.m_arr1) && (this->m_index == rhs.m_index);
}

template <typename E1, typename Op>
bool ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator!=(const_iterator const & rhs) {
    return !(*this == rhs);
}

template <typename E1, typename Op>
bool ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator<(const_iterator const & rhs) {
    return this->m_index < rhs.m_index;
}

/* Evaluate Operators */
template <typename E1, typename Op>
typename ValarrayExpr<E1, VoidExpr, Op>::value_type ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator*() const {
    return (*this)[this->m_index];
}

template <typename E1, typename Op>
typename ValarrayExpr<E1, VoidExpr, Op>::value_type ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator[](uint64_t k) const {
    return this->op(this->m_arr1[k]);
}

/* Arithemtic Operators: +, -, ++, -- */
template <typename E1, typename Op>
typename ValarrayExpr<E1, VoidExpr, Op>::const_iterator ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator+(difference_type k) {
    return const_iterator(this->m_arr1, this->m_arr2, this->op, this->m_index + k);
}

template <typename E1, typename E2, typename Op, typename N>
typename ValarrayExpr<E1, VoidExpr, Op>::const_iterator operator+(N const & k, typename ValarrayExpr<E1, VoidExpr, Op>::const_iterator const & it) {
    return ValarrayExpr<E1, VoidExpr, Op>::const_iterator(it.m_arr1, it.m_arr2, it.op, it.index + k);
}

template <typename E1, typename Op>
typename ValarrayExpr<E1, VoidExpr, Op>::const_iterator ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator-(difference_type k) {
    return const_iterator(this->m_arr1, this->m_arr2, this->op, this->m_index - k);
}

template <typename E1, typename Op>
typename ValarrayExpr<E1, VoidExpr, Op>::const_iterator::difference_type ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator-(const_iterator const & rhs) {
    return this->m_index - rhs.m_index;
}

template <typename E1, typename Op>
typename ValarrayExpr<E1, VoidExpr, Op>::const_iterator ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator++() {
    ++this->m_index;
    return *this;
}

template <typename E1, typename Op>
typename ValarrayExpr<E1, VoidExpr, Op>::const_iterator ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator++(int) {
    const_iterator it(*this);
    ++(*this);
    return it;
}

template <typename E1, typename Op>
typename ValarrayExpr<E1, VoidExpr, Op>::const_iterator ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator--() {
    --this->m_index;
    return *this;
}

template <typename E1, typename Op>
typename ValarrayExpr<E1, VoidExpr, Op>::const_iterator ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator--(int) {
    const_iterator it(*this);
    --(*this);
    return it;
}

/* Compound Arithmetic Operators: +=, -= */
template <typename E1, typename Op>
typename ValarrayExpr<E1, VoidExpr, Op>::const_iterator ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator+=(difference_type k) {
    return *this + k;
}

template <typename E1, typename Op>
typename ValarrayExpr<E1, VoidExpr, Op>::const_iterator ValarrayExpr<E1, VoidExpr, Op>::const_iterator::operator-=(difference_type k) {
    return *this - k;
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
    ValarrayExpr<valarray<T>, ValarrayExpr<E1, E2, Op>, std::plus <ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>>
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
    ValarrayExpr<valarray<T>, ValarrayExpr<E1, E2, Op>, std::minus <ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>>
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
    ValarrayExpr<valarray<T>, ValarrayExpr<E1, E2, Op>, std::multiplies <ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>>
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
    ValarrayExpr<valarray<T>, ValarrayExpr<E1, E2, Op>, std::divides <ChooseType<T, ChooseType<typename E1::value_type, typename E2::value_type>>>>
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

/* Accumulate - Applies function to all elements. */
template <typename T>
template <typename Funct>
T valarray<T>::accumulate(Funct funct) {
    if (this->size() == 0) { return (T)0; }
    if (this->size() == 1) { return (*this)[0]; }
    T result = (*this)[0];
    for (uint64_t i = 1; i < this->size(); i++) {
        result = funct(result, (*this)[i]);
    }
    return result;
}

/* Sum - Adds up all elements in valarray. */
template <typename T>
T valarray<T>::sum() {
    return this->accumulate(std::plus<T>{});
}

/* Apply - Applies function to all elements. */
template <typename T>
template <typename Funct>
ValarrayExpr<valarray<T>, VoidExpr, Funct> valarray<T>::apply(Funct funct) {
    ValarrayExpr<valarray<T>, VoidExpr, Funct> result(*this, VoidExpr{}, funct);
    return result;
}

/* Sqrt */
template <typename T>
ValarrayExpr<valarray<T>, VoidExpr, Sqrt<T>> valarray<T>::sqrt() {
    return this->apply(Sqrt<T>{});
}


/*
 * Sqrt Function
 */
template <typename T>
double Sqrt<T>::operator()(T const & val) const {
    return std::sqrt(val);
}
template <typename T>
std::complex<double> Sqrt<std::complex<T>>::operator()(std::complex<T> const & val) const {
    return std::sqrt(val);
}


#endif /* _Valarray_h */

