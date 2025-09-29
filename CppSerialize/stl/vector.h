#pragma once

#include "../layout_traits.h"

#include <vector>


namespace CppSerialize {


template<class T>
struct layout_traits<std::vector<T>> {
	constexpr static layout_size size() {
		return layout_size_dynamic;
	}
	constexpr static void read(auto f, const auto& object) {
		f(object.size()); for (auto& item : object) { f(item); }
	}
	constexpr static void write(auto f, auto& object) {
		size_t count; f(count); object.resize(count); for (auto& item : object) { f(item); }
	}
};


}