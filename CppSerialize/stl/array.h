#pragma once

#include "../layout_traits.h"

#include <array>


namespace CppSerialize {


template<class T>
struct layout_traits<std::array<T, 0>> {
	constexpr static layout_size size() { return layout_size_empty; }
	constexpr static void read(auto f, const auto& object) {}
	constexpr static void write(auto f, auto& object) {}
};

template<class T, size_t count> requires layout_trivial<T>
struct layout_traits<std::array<T, count>> : layout_traits_trivial<std::array<T, count>> {};

template<class T, size_t count>
struct layout_traits<std::array<T, count>> {
	constexpr static layout_size size() {
		return count * layout_traits<T>::size();
	}
	constexpr static void read(auto f, const auto& object) {
		for (auto& item : object) { f(item); }
	}
	constexpr static void write(auto f, auto& object) {
		for (auto& item : object) { f(item); }
	}
};


}