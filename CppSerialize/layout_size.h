#pragma once

#include "layout.h"

#include <cstddef>
#include <cstdint>


namespace CppSerialize {


struct layout_size {
	size_t size;

	constexpr layout_size(size_t size) : size(size) {}

	template<class T>
	constexpr layout_size(layout_type<T>);

	constexpr operator size_t() const { return size; }
};

constexpr layout_size layout_size_empty = { 0 };
constexpr layout_size layout_size_dynamic = { SIZE_MAX };

constexpr bool operator<(layout_size a, layout_size b) {
	return a.size < b.size;
}

constexpr layout_size operator+(layout_size a, layout_size b) {
	if (a.size > layout_size_dynamic.size - b.size) {
		return layout_size_dynamic;
	}
	return { a.size + b.size };
}

constexpr layout_size operator*(size_t count, layout_size b) {
	if (count > layout_size_dynamic.size / b.size) {
		return layout_size_dynamic;
	}
	return { count * b.size };
}


}