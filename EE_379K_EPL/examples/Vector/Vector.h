#pragma once
#include <cstdint>


template <typename T>
class Vector {
private:
	T* data;
	uint64_t length;
	uint64_t capacity;

	static constexpr uint64_t default_capacity = 8;
public:
	Vector(void) {
		data = new T[default_capacity];
		length = 0;
		capacity = default_capacity;
	}
};
