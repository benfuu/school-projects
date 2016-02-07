/*
	Vector.h -- header file for Vector data structure project
	Author: Ben Fu (byf69)
*/

#pragma once
#ifndef _Vector_h
#define _Vector_h

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <stdexcept>

#define MIN_CAPACITY 8

using std::cout;
using std::endl;
using std::allocator;

namespace epl {
	

template <typename T>
class vector {
  public:
	/* Constructors */
	vector();
	vector(uint64_t n);
	vector(vector const &);	// copy constructor
	vector(vector &&);		// move constructor
	~vector();					// destructor
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
	// void push_back(T &&);
	void push_front(T const &);
	// void push_front(T &&);
	void pop_back();
	void pop_front();

  private:
	/* Members */
	T* elems = nullptr;
	uint64_t m_size = 0;
	uint64_t m_capacity = 0;	// size <= capacity
	uint64_t m_first_elem = 0;	// index of first element in the array
	allocator<T> al;
	/* Private functions */
	void copy(vector const &);
	void move(vector &&);
	void destroy();
};

/* Constructors */
template <typename T>
vector<T>::vector() {
	this->elems = al.allocate(MIN_CAPACITY);
	this->m_capacity = MIN_CAPACITY;
}

template <typename T>
vector<T>::vector(uint64_t n) {	// creates an empty vector with capacity n
	if (n == 0) {	// same as no-argument constructor
		this->elems = al.allocate(MIN_CAPACITY);
		this->m_capacity = MIN_CAPACITY;
	}
	else if (n < MIN_CAPACITY) {	// same as above, but initialize n elements, size = n
		this->elems = al.allocate(MIN_CAPACITY);
		this->m_size = n;
		this->m_capacity = MIN_CAPACITY;
		for (uint64_t i = 0; i < n; i++) {	// run default constructor on n elements
			al.construct(&(*this)[i]);
			// al.construct(&this->elems[this->first_elem() + i]);
		}
	}
	else {	// allocate memory for and run constructor on n elements, set size = capacity = n
		this->elems = al.allocate(n);
		this->m_size = n;
		this->m_capacity = n;
		for (uint64_t i = 0; i < n; i++) {
			al.construct(&(*this)[i]);
			// al.construct(&this->elems[this->first_elem() + i]);
		}
	}
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
	if (this->capacity() > 0) { this->destroy(); }
	this->m_size = 0;
	this->m_capacity = 0;
	this->m_first_elem = 0;
}

/* Operators */
template <typename T>
vector<T>& vector<T>::operator=(vector const & rhs) {
	if (this != &rhs) {
		if (this->capacity() > 0) { this->destroy(); }
		this->copy(rhs);
	}
	return *this;
}

template <typename T>
vector<T>& vector<T>::operator=(vector && rhs) {
	if (this != &rhs) {
		if (this->capacity() > 0) { this->destroy(); }
		this->move(std::move(rhs));
	}
	return *this;
}


/* Returns k-th element of vector. Throws out of range if k < 0 or >= size */
template <typename T>
T& vector<T>::operator[](uint64_t k) {
	if ((k > 0 && k >= this->size()) || k < 0) { throw std::out_of_range("index out of range"); }
	return this->elems[this->first_elem() + k];
}
template <typename T>
T const& vector<T>::operator[](uint64_t k) const {	// const version of the above function
	if ((k > 0 && k >= this->size()) || k < 0) { throw std::out_of_range("index out of range"); }
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
	return this->m_first_elem;
}

/* Functions */
/* Inserts a new element to end of array. Uses amortized doubling when needed */
template <typename T>
void vector<T>::push_back(T const & elem) {
	if (this->first_elem() + this->size() == this->capacity()) {	// check for capacity in back
		this->m_capacity *= 2;	// double the current capacity
		T* arr = al.allocate(this->capacity());	// allocate memory for new array
		for (uint64_t i = 0; i < this->size(); i++) {	// copy current contents into new array
			al.construct(&arr[this->first_elem() + i], (*this)[i]);
		}
		this->destroy();
		this->elems = arr;		// set pointer to elements to the new array
	}
	this->m_size += 1;
	al.construct(&(*this)[this->size() - 1], elem);
	// al.construct(&this->elems[this->first_elem() + this->size()], elem);	// construct element at end of array
}
/*
template <typename T>
void vector<T>::push_back(T && elem) {}
*/

/* Inserts a new element to beginning of vector. */
template <typename T>
void vector<T>::push_front(T const & elem) {
	if (this->first_elem() == 0) {	// out of capacity at beginning of array
		uint64_t old_capacity = this->capacity();
		this->m_capacity *= 2;	// double the current capacity
		T* arr = al.allocate(this->capacity());		// allocate memory for new array
		for (uint64_t i = 0; i < this->size(); i++) {	// copy current contents into new array
			al.construct(&arr[this->capacity() - old_capacity + i], (*this)[i]);
		}
		this->destroy();
		this->elems = arr;		// set pointer to elements to the new array
		this->m_first_elem = this->capacity() - old_capacity;		// first element is halfway through the new array
	}
	this->m_first_elem -= 1;
	this->m_size += 1;
	al.construct(&(*this)[0], elem);
	// al.construct(&this->elems[this->first_elem() - 1], elem);	// construct element at beginning of array
}

/*
template <typename T>
void vector<T>::push_front(T && elem) {}
*/

/* Removes object at end of array and update size. Does NOT deallocate memory */
template <typename T>
void vector<T>::pop_back() {
	if (this->size() == 0) { throw std::out_of_range("no elements to pop back"); }
	al.destroy(&(*this)[this->size() - 1]);
	// al.destroy(&this->elems[this->first_elem() + this->size() - 1]);
	this->m_size -= 1;
}

/* Removes object at beginning of array and update size. Does NOT deallocate memory */
template <typename T>
void vector<T>::pop_front() {
	if (this->size() == 0) { throw std::out_of_range("no elements to pop front"); }
	else if (this->size() == 1) {
		this->pop_back();
	}
	else {
		al.destroy(&(*this)[0]);
		// al.destroy(&this->elems[this->first_elem()]);
		this->m_first_elem += 1;	// increment the index of the first element
		this->m_size -= 1;
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
		// al.construct(&this->elems[this->first_elem() + i], source[i]);
	}
}

template <typename T>
void vector<T>::move(vector && source) {
	this->elems = source.elems;
	this->m_size = source.m_size;
	this->m_capacity = source.m_capacity;
	this->m_first_elem = source.first_elem;
	source.elems = nullptr;		// remove reference to the data from source
}


template <typename T>
void vector<T>::destroy() {
	if (this->capacity() == 0) { return; }	// if somehow at 0 capacity, then don't do anything
	else if (this->size() == 0) { al.deallocate(this->elems, this->capacity()); }	// if empty, just deallocate capacity
	else {	// destroy every object and deallocate capacity
		for (uint64_t i = 0; i < this->size(); i++) {
			al.destroy(&(*this)[i]);
			// al.destroy(&this->elems[this->first_elem() + i]);
		}
		al.deallocate(this->elems, this->capacity());
	}
}

}

#endif /* _Vector_h */
