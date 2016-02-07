// String.h -- Simple String example from class

#pragma once

#include <cstdint>
#include <iostream>

class String {
	char* data;
	uint32_t length;
public:
	String(void) {
		data = nullptr;
		length = 0;
	}

	String(char const * src) {
		length = 0;
		while (src[length] != 0) {
			length += 1;
		}
		if (length > 0) {
			data = new char[length];
			for (int k = 0; k < length; k += 1) {
				data[k] = src[k];
			}
		}
		else {
			data = nullptr;
		}
	}

	String(String const& that) { copy(that); }
	~String(void) { destroy(); }
	String& operator=(String const& rhs) {
		if (this != &rhs) {
			destroy();
			copy(rhs);
		}
		return *this;
	}

	String(String&& tmp) {
		this->my_move(std::move(tmp));
	}

	String& operator=(String&& rhs) {
		std::cout << "move assignment!\n";
		std::swap(this->data, rhs.data);
		this->length = rhs.length;
		return *this;
	}

	uint32_t size(void) const { 
		/* NOTE: this is a parameter with the following
		 * effective declaration
		 *   String const* this
		 */
		return this->length; 
	}

	char& operator[](uint32_t k) { return data[k]; }
	char operator[](uint32_t k) const { return data[k]; }

	String& operator+=(String const& rhs) {
		if (rhs.size() == 0) { return *this; } // special case for appending empty

		char const* old_data = data;
		uint32_t old_length = length;

		length += rhs.length;
		data = new char[length];
		for (uint32_t k = 0; k < old_length; k += 1) {
			data[k] = old_data[k];
		}

		for (uint32_t k = 0; k < rhs.length; k += 1) {
			data[k + old_length] = rhs.data[k];
		}

		if (old_data) { delete[] old_data; }

		return *this;
	}

	/* the code below is the general solution to operator+
	 * it almost always provides correct semantics, although it might
	 * not be very efficient. */
	String operator+(String const& right) const {
		String res{ *this };
		res += right;
		std::cout << "result constructed, ready to return\n";
		return res;
	}

	/* because operator+ is so inefficient, I thought I'd show an
	 * example of a more efficient implementation. I would not normally
	 * write both an operator+ and a concat, they are semantically the same thing */
	String concat(String const& right) const {
		if (right.size() == 0) { return String{ *this }; }
		uint32_t new_len = size() + right.size();
		char* new_data = new char[new_len];
		for (uint32_t k = 0; k < size(); k += 1) {
			new_data[k] = this->data[k];
		}
		for (int32_t k = 0; k < right.size(); k += 1) {
			new_data[k + length] = right.data[k];
		}

		/* I've constructed the concatenated string in memory and
		 * I have the pointer and length of that new string, all I need
		 * to do is to return an actual String object (i.e., a struct) 
		 * with the two data components set. I made a private constructor for
		 * that purpose -- private, so that I can use it, but not any application
		 * developers. Note that in the line below, I do not need to indicate
		 * that I'm returning a String{new_data, new_len}, the type is
		 * already known to the compiler (this function returns a String type)
		 * so I only need to provide the arguments to the (private) constructor */
		return { new_data, new_len };
	}

private:
	/* "copy" is what I call the code that implements my
	 * copy constructor functionality. By putting this code in a
	 * private function, I can use that code in my assignment operator
	 * as well as in my copy constructor (or anyplace else I need it) */
	void copy(String const& that) { // WARNING: assumes this is uninitialized */
		this->length = that.length;
		if (length == 0) {
			data = nullptr;
			return;
		}

		data = new char[length];
		for (uint32_t k = 0; k < length; k += 1) {
			data[k] = that.data[k];
		}
	}

	/* destroy is the name I give to my destructor functionality
	 * it's often a very short function (shorter than these comments) */
	void destroy(void) { // WARNING -- turns *this into an un-initialized object
		delete[] data;
	}

	void my_move(String&& tmp) {
		std::cout << "move!\n";
		this->data = tmp.data;
		this->length = tmp.length;
		tmp.data = nullptr;
	}

	String(char* data, uint32_t length) {
		this->data = data;
		this->length = length;
	}
};

