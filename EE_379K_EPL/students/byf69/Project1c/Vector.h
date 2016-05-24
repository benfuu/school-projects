/*
Vector.h -- header file for Vector data structure project
Author: Ben Fu (byf69)
*/

#pragma once
#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <iostream>
#include <initializer_list>

#define MIN_CAPACITY 8

using std::cout;
using std::endl;
using std::allocator;

//Utility gives std::rel_ops which will fill in relational
//iterator operations so long as you provide the
//operators discussed in class.  In any case, ensure that
//all operations listed in this website are legal for your
//iterators:
//http://www.cplusplus.com/reference/iterator/RandomAccessIterator/
using namespace std::rel_ops;

namespace epl {
    template <typename T, typename Container>
    class vector_iterator;  // forward declaration

    template <typename T>
    class vector {
    public:
        template <typename T1, typename Container>
        friend class vector_iterator;
        using iterator = vector_iterator<T, vector<T>>;
        using const_iterator = vector_iterator<T const, vector<T> const>;
        /* Constructors */
        vector();
        vector(uint64_t n);
        vector(vector const &);	// copy constructor
        vector(vector &&);		// move constructor
        template <typename It>
        vector(It, It);   // iterator constructor
        vector(std::initializer_list<T>);  // list constructor
        ~vector();				// destructor
        /* Operators */
        vector& operator=(vector const &);
        vector& operator=(vector &&);
        T& operator[](uint64_t);
        T const & operator[](uint64_t) const;
        /* Getters */
        uint64_t size() const;
        uint64_t capacity() const;
        uint64_t first_elem() const;
        /* Functions */
        void push_back(T const &);
        void push_back(T &&);
        void push_front(T const &);
        void push_front(T &&);
        void pop_back();
        void pop_front();
        template <typename ...Args>
        void emplace_back(Args... args);
        /* Iterator Functions */
        iterator begin();
        const_iterator begin() const;
        iterator end();
        const_iterator end() const;

    private:
        /* Members */
        T* elems = nullptr;
        uint64_t m_size = 0;
        uint64_t m_capacity = 0;	// size <= capacity
        uint64_t m_first_elem = 0;	// index of first element in the array
        allocator<T> al;
        bool iterator_valid = true; // keeps track of possible iterator corruption
        bool iterator_moved = false;    // keeps track if memory reallocation or vector reassignment was performed
        vector<iterator*>* iterators;  // keeps a reference to all the iterators
        vector<const_iterator*>* const_iterators;
        /* Private functions */
        void copy(vector const &);
        void move(vector &&);
        void destroy();
    };

    template <typename T, typename Container>
    class vector_iterator : public std::iterator<std::random_access_iterator_tag, T> {
    public:
        using difference_type = std::ptrdiff_t;
        /* Constructors */
        vector_iterator();
        vector_iterator(vector_iterator const &);   // copy constructor
        vector_iterator(T const *);
        vector_iterator(T const *, Container const *);
        vector_iterator(vector<T> const * const); // vector constructor
        ~vector_iterator();
        /* Operators */
        template <typename S, typename SContainer>
        vector_iterator& operator=(vector_iterator<S, SContainer> const &);
        operator vector_iterator<T const, Container const>() const;
        template <typename S, typename SContainer>
        bool operator==(vector_iterator<S, SContainer> const &) const;
        template <typename S, typename SContainer>
        bool operator<(vector_iterator<S, SContainer> const &) const;
        T& operator*() const;
        T* operator->() const;
        vector_iterator& operator++();      // pre-increment
        vector_iterator operator++(int);    // post-increment
        vector_iterator& operator--();      // pre-decrement
        vector_iterator operator--(int);    // post-decrement
        vector_iterator operator+(difference_type);
        template <typename S, typename SContainer>
        friend vector_iterator<S, SContainer> operator+(difference_type, vector_iterator<S, SContainer> const &);
        vector_iterator operator-(difference_type);
        template <typename S, typename SContainer>
        friend vector_iterator<S, SContainer> operator-(difference_type, vector_iterator<S, SContainer> const &);
        template <typename S, typename SContainer>
        difference_type operator-(vector_iterator<S, SContainer> &);
        vector_iterator& operator+=(difference_type);
        vector_iterator& operator-=(difference_type);
        T& operator[](difference_type);
        T const & operator[](difference_type) const;
        /* Swap Function */
        void swap(vector_iterator&);
        /* Getters/Setters */
        T* get_ptr() const;
        void set_ptr(T const *);

        template <typename S>
        friend class vector;
        template <typename S, typename SContainer>
        friend class vector_iterator;
    private:
        T* m_ptr = nullptr;
        Container*  m_vector_ptr;
        void check_valid();
        void check_valid() const;
    };

    class invalid_iterator {
    public:
        enum SeverityLevel { SEVERE, MODERATE, MILD, WARNING };
        SeverityLevel level;

        invalid_iterator(SeverityLevel level = SEVERE) { this->level = level; }
        virtual const char* what() const {
            switch (level) {
            case WARNING:   return "Warning"; // not used in Spring 2015
            case MILD:      return "Mild";
            case MODERATE:  return "Moderate";
            case SEVERE:    return "Severe";
            default:        return "ERROR"; // should not be used
            }
        }
    };
    

    /*
     * Vector Member Functions
     */

    /* Constructors */
    template <typename T>
    vector<T>::vector() {
        this->elems = al.allocate(MIN_CAPACITY);
        this->m_capacity = MIN_CAPACITY;
    }

    template <typename T>
    vector<T>::vector(uint64_t n) : vector() {	// creates an empty vector with capacity n
        if (n == 0) {}	// same as no-argument constructor
        else if (n < MIN_CAPACITY) {	// same as above, but initialize n elements, size = n
            this->elems = al.allocate(MIN_CAPACITY);
            this->m_size = n;
            this->m_capacity = MIN_CAPACITY;
            for (uint64_t i = 0; i < n; i++) {	// run default constructor on n elements
                al.construct(&(*this)[i]);
            }
        }
        else {	// allocate memory for and run constructor on n elements, set size = capacity = n
            this->elems = al.allocate(n);
            this->m_size = n;
            this->m_capacity = n;
            for (uint64_t i = 0; i < n; i++) {
                al.construct(&(*this)[i]);
            }
        }
    }

    template <typename T>
    template <typename It>
    vector<T>::vector(It b, It e) : vector() {
        while (b != e) {
            this->push_back(*b);
            b++;
        }
        this->iterator_valid = true;    // no iterators were really changed
    }

    template <typename T>
    vector<T>::vector(std::initializer_list<T> list) : vector(list.begin(), list.end()) {

    }

    /* Copy constructors */
    template <typename T>
    vector<T>::vector(vector const & obj) {
        if (this == &obj) { return; }
        this->copy(obj);
    }


    template <typename T>
    vector<T>::vector(vector && obj) {
        if (this == &obj) { return; }
        this->move(std::move(obj));
    }


    /* Destructor */
    template <typename T>
    vector<T>::~vector() {	// destroy elements, deallocate memory, and set members to 0
        this->destroy();
        //this->m_size = 0;
        //this->m_capacity = 0;
        //this->m_first_elem = 0;
    }

    /* Operators */
    template <typename T>
    vector<T>& vector<T>::operator=(vector const & rhs) {
        if (this != &rhs) {
            this->iterator_valid = false;
            this->iterator_moved = true;
            this->copy(rhs);
        }
        return *this;
    }

    template <typename T>
    vector<T>& vector<T>::operator=(vector && rhs) {
        if (this != &rhs) {
            this->iterator_valid = false;
            this->iterator_moved = true;
            if (this->capacity() > 0) { this->destroy(); }
            this->move(std::move(rhs));
        }
        return *this;
    }


    /* Returns k-th element of vector. Throws out of range if k < 0 or >= size */
    template <typename T>
    T& vector<T>::operator[](uint64_t k) {
        if (k >= this->size() || k < 0) { throw std::out_of_range("index out of range"); }
        return this->elems[this->first_elem() + k];
    }
    template <typename T>
    T const& vector<T>::operator[](uint64_t k) const {	// const version of the above function
        if (k >= this->size() || k < 0) { throw std::out_of_range("index out of range"); }
        return this->elems[this->first_elem() + k];
    }

    /* Getters */
    /* Returns size of vector. */
    template <typename T>
    uint64_t vector<T>::size() const {
        return this->m_size;
    }

    /* Returns capacity of vector. Note that capacity >= size. */
    template <typename T>
    uint64_t vector<T>::capacity() const {
        return this->m_capacity;
    }

    /* Returns the index of the first element of the vector. */
    template <typename T>
    uint64_t vector<T>::first_elem() const {
        if (!this) { return 0; }
        return this->m_first_elem;
    }

    /* Functions */
    /* Inserts a new element to end of array. Uses amortized doubling when needed */
    template <typename T>
    void vector<T>::push_back(T const & elem) {
        this->iterator_valid = false;
        if ((this->first_elem() + this->size()) == this->capacity()) {	// check for capacity in back
            this->iterator_moved = true;
            T* arr = al.allocate(this->capacity() * 2);	// allocate memory for new array
            for (uint64_t i = 0; i < this->size(); i++) {	// move current contents into new array
                al.construct(&arr[this->first_elem() + i], std::move((*this)[i]));
            }
            al.construct(&arr[this->first_elem() + this->size()], elem);		// construct and copy element to end of array
            this->destroy();		// destroy and deallocate old array
            this->m_capacity *= 2;	// double the current capacity
            this->m_size += 1;		// increment size
            this->elems = arr;		// set pointer to elements to the new array
        }
        else {
            this->m_size += 1;
            al.construct(&(*this)[this->size() - 1], elem);			// construct and copy element to end of array
        }
    }

    template <typename T>
    void vector<T>::push_back(T && elem) {
        this->iterator_valid = false;
        if ((this->first_elem() + this->size()) == this->capacity()) {	// check for capacity in back
            this->iterator_moved = true;
            T* arr = al.allocate(this->capacity() * 2);	// allocate memory for new array
            for (uint64_t i = 0; i < this->size(); i++) {	// move current contents into new array
                al.construct(&arr[this->first_elem() + i], std::move((*this)[i]));
            }
            al.construct(&arr[this->first_elem() + this->size()], std::move(elem));		// construct and move element to end of array
            this->destroy();		// destroy and deallocate old array
            this->m_capacity *= 2;	// double the current capacity
            this->m_size += 1;		// increment size
            this->elems = arr;		// set pointer to elements to the new array
        }
        else {
            this->m_size += 1;
            al.construct(&(*this)[this->size() - 1], std::move(elem));			// construct and move element to end of array
        }
    }


    /* Inserts a new element to beginning of vector. */
    template <typename T>
    void vector<T>::push_front(T const & elem) {
        this->iterator_valid = false;
        if (this->first_elem() == 0) {	// out of capacity at beginning of array
            this->iterator_moved = true;
            T* arr = al.allocate(this->capacity() * 2);		// allocate memory for new array
            uint64_t old_capacity = this->capacity();
            uint64_t new_capacity = this->capacity() * 2;
            for (uint64_t i = 0; i < this->size(); i++) {	// move current contents into new array
                al.construct(&arr[new_capacity - old_capacity + i], std::move((*this)[i]));
            }
            al.construct(&arr[new_capacity - old_capacity - 1], elem);		// construct and copy element to beginning of array
            this->destroy();
            this->m_capacity *= 2;	// double the current capacity
            this->m_size += 1;
            this->m_first_elem = new_capacity - old_capacity - 1;		// first element is halfway through the new array
            this->elems = arr;		// set pointer to elements to the new array
        }
        else {
            this->m_size += 1;
            this->m_first_elem -= 1;
            al.construct(&(*this)[0], elem);		// construct element at beginning of array
        }
    }

    template <typename T>
    void vector<T>::push_front(T && elem) {
        this->iterator_valid = false;
        if (this->first_elem() == 0) {	// out of capacity at beginning of array
            this->iterator_moved = true;
            T* arr = al.allocate(this->capacity() * 2);		// allocate memory for new array
            uint64_t old_capacity = this->capacity();
            uint64_t new_capacity = this->capacity() * 2;
            for (uint64_t i = 0; i < this->size(); i++) {	// move current contents into new array
                al.construct(&arr[new_capacity - old_capacity + i], std::move((*this)[i]));
            }
            al.construct(&arr[new_capacity - old_capacity - 1], std::move(elem));		// construct and move element to beginning of array
            this->destroy();
            this->m_capacity *= 2;	// double the current capacity
            this->m_size += 1;
            this->m_first_elem = new_capacity - old_capacity - 1;		// first element is halfway through the new array
            this->elems = arr;		// set pointer to elements to the new array
        }
        else {
            this->m_size += 1;
            this->m_first_elem -= 1;
            al.construct(&(*this)[0], std::move(elem));		// construct and move element to beginning of array
        }
    }

    /* Removes object at end of array and update size. Does NOT deallocate memory */
    template <typename T>
    void vector<T>::pop_back() {
        this->iterator_valid = false;
        if (this->size() == 0) { throw std::out_of_range("no elements to pop back"); }
        al.destroy(&(*this)[this->size() - 1]);
        this->m_size -= 1;
    }

    /* Removes object at beginning of array and update size. Does NOT deallocate memory */
    template <typename T>
    void vector<T>::pop_front() {
        this->iterator_valid = false;
        if (this->size() == 0) { throw std::out_of_range("no elements to pop front"); }
        else if (this->size() == 1) {
            this->pop_back();
        }
        else {
            al.destroy(&(*this)[0]);
            this->m_first_elem += 1;	// increment the index of the first element
            this->m_size -= 1;
        }

    }

    /* Constructs object at end of array and passes arguments. */
    template <typename T>
    template <typename ...Args>
    void vector<T>::emplace_back(Args... args) {
        this->iterator_valid = false;
        if ((this->first_elem() + this->size()) == this->capacity()) {	// check for capacity in back
            this->iterator_moved = true;
            T* arr = al.allocate(this->capacity());	// allocate memory for new array
            for (uint64_t i = 0; i < this->size(); i++) {	// move current contents into new array
                al.construct(&arr[this->first_elem() + i], std::move((*this)[i]));
            }
            al.construct(&arr[this->first_elem() + this->size()], std::forward<Args>(args)...);		// construct and copy element to end of array
            this->destroy();		// destroy and deallocate old array
            this->m_capacity *= 2;	// double the current capacity
            this->m_size += 1;		// increment size
            this->elems = arr;		// set pointer to elements to the new array
        }
        else {
            this->m_size += 1;
            al.construct(&(*this)[this->size() - 1], std::forward<Args>(args)...);			// construct and copy element to end of array
        }
    }

    /* Copy function */
    template <typename T>
    void vector<T>::copy(vector const & source) {
        if (this->capacity() > 0) { this->destroy(); }
        this->elems = al.allocate(source.capacity());	// allocate space for new array
        this->m_size = source.size();
        this->m_capacity = source.capacity();
        this->m_first_elem = source.first_elem();
        for (uint64_t i = 0; i < source.size(); i++) {		// copy elements
            al.construct(&(*this)[i], source[i]);
        }
    }

    /* Move function */
    template <typename T>
    void vector<T>::move(vector && source) {
        this->elems = source.elems;
        this->m_size = source.size();
        this->m_capacity = source.capacity();
        this->m_first_elem = source.first_elem();
        source.elems = nullptr;		// remove reference to the data from source
    }

    /* Runs destructor on every element in vector and deallocates memory */
    template <typename T>
    void vector<T>::destroy() {
        if (this->elems == nullptr) { return; }	// don't destruct a moved object
        if (this->capacity() == 0) { return; }	// if somehow at 0 capacity, then don't do anything
        if (this->size() == 0) { al.deallocate(this->elems, this->capacity()); }	// if empty, just deallocate capacity
        else {	// destroy every object and deallocate capacity
            for (uint64_t i = 0; i < this->size(); i++) {
                al.destroy(&(*this)[i]);
            }
            al.deallocate(this->elems, this->capacity());
        }
    }

    /* Iterator Functions */
    template <typename T>
    typename vector<T>::iterator vector<T>::begin() {
        iterator it;
        it.m_ptr = &(*this)[0];
        it.m_vector_ptr = this;
        //this->iterator_valid = true;
        //this->iterators->push_back(&it);
        return it;
    }
    template <typename T>
    typename vector<T>::const_iterator vector<T>::begin() const {
        const_iterator it;
        it.m_ptr = &(*this)[0];
        it.m_vector_ptr = this;
        //this->iterator_valid = true;
        //this->const_iterators->push_back(&it);
        return it;
    }
    template <typename T>
    typename vector<T>::iterator vector<T>::end() {
        iterator it;
        it.m_ptr = &(this->elems[this->first_elem() + this->size()]);
        it.m_vector_ptr = this;
        //this->iterator_valid = true;
        //this->iterators->push_back(&it);
        return it;
    }
    template <typename T>
    typename vector<T>::const_iterator vector<T>::end() const {
        const_iterator it;
        it.m_ptr = &(this->elems[this->first_elem() + this->size()]);
        it.m_vector_ptr = this;
        //this->iterator_valid = true;
        //this->const_iterators->push_back(&it);
        return it;
    }



    /*
    * Vector Iterator Member Functions
    */
    //using difference_type = int;
    /* Constructors */
    
    /* Empty Constructor */
    template <typename T, typename Container>
    vector_iterator<T, Container>::vector_iterator() {

    }
    
    /* Copy Constructor */
    template <typename T, typename Container>
    vector_iterator<T, Container>::vector_iterator(vector_iterator const & obj) {
        this->m_ptr = obj.m_ptr;
        this->m_vector_ptr = obj.m_vector_ptr;
    }

    /* Pointer Constructor */
    template <typename T, typename Container>
    vector_iterator<T, Container>::vector_iterator(T const * ptr) {
        this->m_ptr = const_cast<T*>(ptr);
    }

    template <typename T, typename Container>
    vector_iterator<T, Container>::vector_iterator(T const * ptr, Container const * container_ptr) {
        this->m_ptr = const_cast<T*>(ptr);
        this->m_vector_ptr = const_cast<Container*>(container_ptr);
    }

    /* Vector Constructor */
    template <typename T, typename Container>
    vector_iterator<T, Container>::vector_iterator(vector<T> const * const vec) {
        this->m_vector_ptr = vec;
    }

    /* Destructor */
    template <typename T, typename Container>
    vector_iterator<T, Container>::~vector_iterator() {}


    /* Operators */

    /* Assignment Operator */
    template <typename T, typename Container>
    template <typename S, typename SContainer>
    vector_iterator<T, Container>& vector_iterator<T, Container>::operator=(vector_iterator<S, SContainer> const & rhs) {
        rhs.check_valid();
        this->m_ptr = rhs.m_ptr;
        this->m_vector_ptr = rhs.m_vector_ptr;
        return *this;
    }

    template <typename T, typename Container>
    vector_iterator<T, Container>::operator vector_iterator<T const, Container const>() const {
        return vector_iterator<T const, Container const>(this->m_ptr, this->m_vector_ptr);
    }

    /* Equality Checker */
    template <typename T, typename Container>
    template <typename S, typename SContainer>
    bool vector_iterator<T, Container>::operator==(vector_iterator<S, SContainer> const & rhs) const {
        this->check_valid();
        rhs.check_valid();
        return this->m_ptr == rhs.m_ptr;
    }

    /* Less Than Checker */
    template <typename T, typename Container>
    template <typename S, typename SContainer>
    bool vector_iterator<T, Container>::operator<(vector_iterator<S, SContainer> const & rhs) const {
        this->check_valid();
        rhs.check_valid();
        return this->m_ptr < rhs.m_ptr;
    }

    /* Dereference Operators */
    template <typename T, typename Container>
    T& vector_iterator<T, Container>::operator*() const {
        this->check_valid();
        return *(this->m_ptr);
    }
    template <typename T, typename Container>
    T* vector_iterator<T, Container>::operator->() const {
        this->check_valid();
        return this->m_ptr;
    }

    /* Increment */
    template <typename T, typename Container>
    vector_iterator<T, Container>& vector_iterator<T, Container>::operator++() {      // pre-increment
        this->m_ptr++;
        return *this;
    }
    template <typename T, typename Container>
    vector_iterator<T, Container> vector_iterator<T, Container>::operator++(int) {    // post-increment
        vector_iterator it{ *this };
        this->m_ptr++;
        return it;
    }

    /* Decrement */
    template <typename T, typename Container>
    vector_iterator<T, Container>& vector_iterator<T, Container>::operator--() {      // pre-decrement
        this->m_ptr--;
        return *this;
    }
    template <typename T, typename Container>
    vector_iterator<T, Container> vector_iterator<T, Container>::operator--(int) {    // post-decrement
        vector_iterator it{ *this };
        this->m_ptr--;
        return it;
    }

    /* Binary Operators */
    /* Addition */
    template <typename T, typename Container>
    vector_iterator<T, Container> vector_iterator<T, Container>::operator+(difference_type rhs) {    // iterator + number
        return vector_iterator<T, Container>(this->m_ptr + rhs, this->m_vector_ptr);
    }
    template <typename T, typename Container>
    vector_iterator<T, Container> operator+(typename vector_iterator<T, Container>::difference_type lhs, vector_iterator<T, Container> const & rhs) { // number + iterator
        return vector_iterator<T, Container>(lhs + rhs.m_ptr, rhs.m_vector_ptr);
    }
    /* Subtraction */
    template <typename T, typename Container>
    vector_iterator<T, Container> vector_iterator<T, Container>::operator-(difference_type rhs) {    // iterator - number
        return vector_iterator<T, Container>(this->m_ptr - rhs, this->m_vector_ptr);
    }
    template <typename T, typename Container>
    vector_iterator<T, Container> operator-(typename vector_iterator<T, Container>::difference_type lhs, vector_iterator<T, Container> const & rhs) { // number - iterator
        return vector_iterator<T, Container>(lhs - rhs.m_ptr, rhs.m_vector_ptr);
    }
    template <typename T, typename Container>
    template <typename S, typename SContainer>
    typename vector_iterator<T, Container>::difference_type vector_iterator<T, Container>::operator-(vector_iterator<S, SContainer> & rhs) {
        int res = 0;
        if (rhs < *this) {
            for (rhs; rhs != *this; rhs++) {
                res += 1;
            }
        }
        else {
            for (rhs; rhs != *this; rhs--) {
                res -= 1;
            }
        }
        return res;
    }
    /* Complex Operators (+= and -=) */
    template <typename T, typename Container>
    vector_iterator<T, Container>& vector_iterator<T, Container>::operator+=(difference_type rhs) {
        this->m_ptr += rhs;
        return *this;
    }
    template <typename T, typename Container>
    vector_iterator<T, Container>& vector_iterator<T, Container>::operator-=(difference_type rhs) {
        this->m_ptr -= rhs;
        return *this;
    }

    /* Bracket Operator */
    template <typename T, typename Container>
    T& vector_iterator<T, Container>::operator[](difference_type k) {
        return this->m_ptr[k];
    }
    template <typename T, typename Container>
    T const & vector_iterator<T, Container>::operator[](difference_type k) const {
        return this->m_ptr[k];
    }

    /* Swap Function */
    template <typename T, typename Container>
    void vector_iterator<T, Container>::swap(vector_iterator & obj) {
        std::swap(this->m_ptr, obj.m_ptr);
        std::swap(this->m_vector_ptr, obj.m_vector_ptr);
    }

    /* Getters / Setters */
    template <typename T, typename Container>
    T* vector_iterator<T, Container>::get_ptr() const {
        return this->m_ptr;
    }
    template <typename T, typename Container>
    void vector_iterator<T, Container>::set_ptr(T const * ptr) {
        this->m_ptr = ptr;
    }

    /* Exceptions */
    template <typename T, typename Container>
    void vector_iterator<T, Container>::check_valid() {
        if (this->m_ptr < &this->m_vector_ptr->elems[this->m_vector_ptr->first_elem()] && this->m_ptr > &this->m_vector_ptr->elems[this->m_vector_ptr->size()]) { // first check for out of bounds
            throw invalid_iterator{ invalid_iterator::SEVERE };
        }
        bool iterator_valid = this->m_vector_ptr->iterator_valid;
        bool iterator_moved = this->m_vector_ptr->iterator_moved;
        if (!iterator_valid) { // we know iterator is no longer valid
            if (iterator_moved) {
                throw invalid_iterator{ invalid_iterator::MODERATE };
            }
            else {
                throw invalid_iterator{ invalid_iterator::MILD };
            }
        }
    }

    template <typename T, typename Container>
    void vector_iterator<T, Container>::check_valid() const {
        if (this->m_ptr < &this->m_vector_ptr->elems[this->m_vector_ptr->first_elem()] && this->m_ptr > &this->m_vector_ptr->elems[this->m_vector_ptr->size()]) { // first check for out of bounds
            throw invalid_iterator{ invalid_iterator::SEVERE };
        }
        bool iterator_valid = this->m_vector_ptr->iterator_valid;
        bool iterator_moved = this->m_vector_ptr->iterator_moved;
        if (!iterator_valid) { // we know iterator is no longer valid
            if (iterator_moved) {
                throw invalid_iterator{ invalid_iterator::MODERATE };
            }
            else {
                throw invalid_iterator{ invalid_iterator::MILD };
            }
        }
    }

} //namespace epl

#endif
