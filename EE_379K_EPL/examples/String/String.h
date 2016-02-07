/*
 * String.h
 *
 *  Created on: Jan 22, 2015
 *      Author: chase
 */

/* the CPP directives below (i.e., the lines starting with "#") represent a fairly standard 
 * opening to a C/C++ header file. The directives are as follows
 *
 *     #ifndef STRING_H_
 *     #define STRING_H_
 * These two lines (and the #endif /* STRING_H_) at the end of this file are intended to ensure
 * that the contents of this header file are included into a program at most one time.
 * In large software systems, many files #include many other files. It is often the case that
 * the same file can be included more than once. For example, I might build a class Customer.h
 * that relies on Strings, and then write a program that works with both Strings and Customers
 * My program would typically include String.h and Customer.h. However, since Customer.h depends
 * on String.h, the Customer.h file may also include String.h -- resulting in String.h being
 * included twice into my program. Re-definitions of structs in C/C++ can cause programs, so we
 * seek to prevent the multiple inclusion. The #ifndef directive is "if not defined". We provide
 * this directive with an arbitrary macro (STRING_H_ in this case), which we expect to NOT
 * be defined. We usually use the name of the file itself (String.h) to construct the arbitrary
 * macro to ensure that the macro was defined in some other file. Since STRING_H_ is not defined
 * the #ifndef directive evaluates affirmatively and allows the contents of the file to be included
 * (up to the matching #endif at the end of this file). HOWEVER, note that when the contents are
 * included, we get a definition of the STRING_H_ macro. We define that macro to be empty,
 * but the contents of the macro are immaterial -- the macro is now defined (defined to be empty)
 * Note that some programmers prefer to define their macro to be something (i.e,. not empty) and 
 * the most common practice among those programmers is to define the macro to be the constant 1
 * e.g., you'll see a lot of codes that have the second directive looking like this
 *
 *      #define STRING_H_ 1
 *
 * Finally, many compilers are modified to provide one-time-only inclusion of files (including
 * gcc/g++ and Visual Studio). For those compilers, a simple directive
 * 
 *      #pragam once
 *
 * is all that is required (do not need the #ifndef at all). However, most programmers still use
 * the #ifndef technique for backwards compatibility
 *
 * The remainder of the directives at the top of this file are three #include directives specifying
 * commonly used library components.
 *
 *    #include <cstdint> -- includes a file with typedef statements definining int32_t, int64_t and similar
 *    #include <iostream> -- includes a file with declarations of the C++ input/output library
 *    #include <stdexcept> -- include a file with definitions of standard exception types in C++
 *
 * Most programs I write will include the first two (cstdint and iostream). The order of these includes
 * does not matter. I will include stdexcept only if my code includes exception handling (either throw
 * or catch statements)
 * 
 */

#ifndef STRING_H_
#define STRING_H_
#include <cstdint>
#include <iostream>
#include <stdexcept>

/*
 * This String example assumes that the underlying computer uses little-endian byte ordering for
 * multi-byte words. That is, when a multi-byte integer (e.g., uint64_t) is stored in memory, the
 * least significant byte (i.e, the least-signficant eight bits of the number) is stored in the
 * first byte in memory, the second-least-significant byte is stored second and so forth.
 *
 * In this implementation, the storage for characters is aligned to an eight-byte boundary and padded
 * to be a multiple of eight bytes. The character data is written "backwards", starting from the last
 * byte of the storage area and working in reverse order. Any unused bytes in the storage area are
 * set to zero. The reason for using this implementation is that when the last word of the storage
 * area is read as a uint64_t, the value of that integer is constructed by taking the first character
 * in the string (which is stored in the last byte of the storage area) and using that character's
 * byte encoding (e.g., its ASCII value) as the most significant byte of the integer. If two uint64_t
 * integers are read in this fashion, one integer from each of two strings, then comparing those
 * integers produces an identical result to comparing the first eight characters in the strings.
 * Note that since the storage areas are padded with zeros, prefix strings are compared correctly
 * as well. The string "abcdef" uses only six bytes with two bytes of zero. If that string were
 * compared to "abcdefaa", then the former string is "less than" the latter because of the zeros loaded
 * into the least significant bit positions.
 *
 * The implementation does not guarantee that character strings a null terminated. Doing so serves
 * no purpose since with character data written in reverse, the strings cannot be used as if they were
 * normal C strings anyway.
 */
class String {
private:
	/* this implementation uses 8-byte, unsigned words */
	static constexpr uint32_t bytes_per_word = 8;
	using word_t = uint64_t;

	word_t* storage; // pointer to the beginning of the storage area (end of the character string)
	uint32_t storage_size; // number of words in the storage array

	uint32_t len; // number of characters stored in the storage area
	char* data; // pointer to the end of the storage array
	/* character data is stored at data[-k] for k = 1, 2, ... len
	 * Note that we used negative indices for characters and we're 1 based rather than zero
	 */

	/* a helper function to convert from bytes to words, note that we need to round up */
	static constexpr uint32_t word_len(uint32_t byte_len) {
		return (byte_len + bytes_per_word - 1) / bytes_per_word;
	}

	template <typename Character> class iterator_helper {
	private:
		Character* ptr;
		using Same = iterator_helper<Character>;
	public:
		Character& operator*(void) { return ptr[-1];}
		Same& operator++(void) {
			--ptr;
			return *this;
		}
		Same operator++(int) {
			Same t{*this}; // make a copy
			operator++(); // increment myself
			return t;
		}
		Same operator+(int32_t k) {
			Same result{};
			result.ptr = this->ptr - k;
		}
	};

public:
	using iterator=iterator_helper<char>;
	using const_iterator=iterator_helper<const char>;

	String(void) {
		storage = nullptr;
		data = nullptr;
		storage_size = 0;
		len = 0;
	}

	String(const char* ptr) {
		len = 0;
		while (ptr[len]) { len += 1; }

		storage_size = word_len(len);
		storage = new word_t[storage_size];

		*storage = 0; // ensure the end of the string will be padded with zeros

		data = reinterpret_cast<char*>(storage + storage_size);
		for (int32_t k = 1; k <= (int32_t) len; k += 1) {
			data[-k] = ptr[k-1];
		}
	}

	/* yuck! bad idea to have type conversions both from and to char*
	 * (and this one has a memory leak)
	 */
//	operator const char*(void) const {
//		char* res = new char[len+1];
//		for (uint32_t k = 0; k < len; k += 1) {
//			res[k] = (*this)[k];
//		}
//		res[len] = 0;
//		return res;
//	}

	~String(void) { destroy(); }

	String(const String& that) { copy(that); }

	String& operator=(const String& rhs) {
		if (this != &rhs) {
			destroy();
			copy(rhs);
		}
		return *this;
	}

	char& operator[](uint32_t k) {
		if (k >= len) { throw std::out_of_range{"index out of range"}; }
		int32_t i = (int32_t) k;
		return data[-(i+1)];
	}

	char operator[](uint32_t k) const {
		if (k >= len) { throw std::out_of_range{"index out of range"}; }
		int32_t i = (int32_t) k;
		return data[-(i+1)];
	}

	uint32_t size(void) const { return len; }

	void print(std::ostream& out) const {
		for (int32_t k = 1; k <= len; k += 1) {
			out << data[-k];
		}
	}

//	bool operator==(const String& rhs) const {
	bool isEqual(const String& rhs) const {
		const String& lhs = *this;
		if (rhs.len != lhs.len) { return false; }
		for (uint32_t k = 0; k < lhs.storage_size; k += 1) {
			if (storage[k] != rhs.storage[k]) { return false; }
		}
	}

	bool operator<(const String& rhs) const {
		const String& lhs = *this;

		uint32_t k = 1;
		uint32_t lhs_words = lhs.storage_size;
		uint32_t rhs_words = rhs.storage_size;
		while (k <= lhs_words && k <= rhs_words
				&& lhs.storage[lhs_words-k] == rhs.storage[rhs_words-k]) {
			k += 1;
		}
		if (k > rhs_words) { return false; }
		if (k > lhs_words) { return true; }
		return lhs.storage[lhs_words-k] < rhs.storage[rhs_words-k];
	}

//	bool operator!=(const String& rhs) const { const String& lhs = *this; return !(lhs == rhs); }
//	bool operator>(const String& rhs) const { const String& lhs = *this; return rhs < lhs; }
//	bool operator<=(const String& rhs) const { const String& lhs = *this; return !(rhs < lhs); }
//	bool operator>=(const String& rhs) const { const String& lhs = *this; return !(lhs < rhs); }

	iterator begin(void) {
	}

private:
	void copy(const String& that) {
		len = that.len;
		if (that.storage) {
			storage_size = word_len(len);
			storage = new word_t[storage_size];
			for (uint32_t k = 0; k < storage_size; k += 1) {
				storage[k] = that.storage[k];
			}
			data = reinterpret_cast<char*>(storage + storage_size);
		} else {
			storage = nullptr;
			data = nullptr;
		}
	}
	void destroy(void) {
		delete[] storage;
	}
};

inline bool operator==(const String& s1, const String& s2) {
	return s1.isEqual(s2);
}

inline std::ostream& operator<<(std::ostream& out, const String& s) {
	s.print(out);
	return out;
}





#endif /* STRING_H_ */
