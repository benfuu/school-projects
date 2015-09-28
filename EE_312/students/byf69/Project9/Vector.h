// Vector.h -- specification of the Vector abstract data type
//#ifndef _Vector_h
//#define _Vector_h
#include <stdint.h>
#include <stdio.h>
#include <stdexcept>


/* The Vector we do as our C-language case study is limited to simulating
 * an array of integers (type int32_t). When we redo this data structure
 * in C++, we'll make a more useful Vector that can simulate an array of 
 * of anything. Also, as we're limited to C, this data structure will be
 * somewhat clunky to use, again, we'll fix that when we have the C++ tools
 *
 *
 * A Vector is always implemented using a dynamically allocated array
 * in practice, the abstract data type (i.e., the "class") for Vector is
 * just an interface wrapper around dynamically allocated arrays. The 
 * "improved" interface we provide includes
 *
 * 1. Bounds checking (no more buffer overlows creeping into production code)
 * 2. Adjustable size -- we can increase or decrease the size by adding or
 *    removing elements at the end of the array.
 *
 * In C, there are some stiff penalties (that clunkiness I talked about) to pay
 * to get these features. Most notably, it becomes incumbant upon our clients
 * (anyone who writes code using our Vector) to ensure that Vectors are always
 * properly initialized and always properly destroyed.
 *
 * The list of functions below is undoubtedly incomplete for a real-world
 * abstract data type. However, it is sufficient to do some basic ops on
 * Vectors. Recall that the "implied contract" states that client code will
 * NOT access the components inside the Vector struct (ever). Therefore 
 * anything that a client programmer might want to do must be provided by
 * the functions below
 */

/* T is a "type placeholder" 
 * in order to use the Vector, we simply tell the compiler what we want 
 * T to be (e.g., "int" or "Customer") and the compiler replaces T 
 * everywhere with that type and then compiles it. 
 * 
 * Using a template means we don't have to make a new Vector implementation 
 * for each possible way that our clients want to use the Vector
 */
template <typename T>
struct Vector {
private:
	uint32_t len;
	uint32_t capacity;
	T* data;

	const uint32_t minimum_capacity = 8;
public:
	/* Constructors */
	Vector(uint32_t init_size) {
		init(init_size);
		for (uint32_t k = 0; k < len; k += 1) {
			T val{}; // default construct a value of type T
			data[k] = val;
		}
	}

	Vector(void) {
		init(0);
	}

	/* Destructor */
	~Vector(void) { 
		delete [] data; 
	}

	/* Copy and assignment */
	Vector(const Vector<T>& that) { copy(that); }

	Vector<T>& operator=(const Vector<T>& rhs) {
		if (this != &rhs) { // protect against someone doing "x = x";
			this->destroy();
			this->copy(rhs);
		}
		return *this;
	}

	/* simple data access methods */
	uint32_t size(void) const { return len; }

	T& operator[](uint32_t k) {
		if (k >= len) { // client error!
			barf("vector index out of bounds");
		}
		return data[k];
	}

	void push_back(const T& val) {
		if (len == capacity) {
			capacity *= 2;
			T* old_data = data;
			data = new T[capacity];
			for (uint32_t k = 0; k < len; k += 1) {
				data[k] = old_data[k];
			}
			delete [] old_data;
		}
		data[len] = val;
		len += 1;
	}

	void pop_back(void) {
		if (len == 0) { // client error!
			barf("pop on empty vector");
		}
		len -= 1;
	}
	
private:
	void init(uint32_t init_size) {
		len = init_size;
		capacity = init_size;
		if (capacity < minimum_capacity) { capacity = minimum_capacity; }

		data = new T[capacity];
	}


	/* copy assumes that "this" is uninitialized
	 * and copy will initialize this to be a true
	 * duplicate of "that") */
	void copy(const Vector<T>& that) {
		init(that.len);
		for (uint32_t k = 0; k < that.len; k += 1) {
			this->data[k] = that.data[k];
		}
	}

	/* destroy assumes that the vector is initialized and it returns
	* the vector to an uninitialized state */
	void destroy(void) {
		delete [] data;
	}

	void barf(const char* msg) {
		printf("FATAL ERROR %s\n", msg);
		throw std::exception{};
	}
};
//#endif

