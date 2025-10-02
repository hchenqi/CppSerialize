#pragma once

#include "../layout_traits.h"

#include <optional>


namespace CppSerialize {


template<class T>
struct layout_traits<std::optional<T>> {
	constexpr static layout_size size() {
		if constexpr (layout_traits<T>::size() == layout_size_empty) {
			return { sizeof(bool) };
		} else {
			return layout_size_dynamic;
		}
	}
	constexpr static void read(auto f, const auto& object) {
		f(object.has_value()); if (object.has_value()) { f(object.value()); }
	}
	constexpr static void write(auto f, auto& object) {
		bool has_value; f(has_value); if (has_value) { object = T(); f(object.value()); }
	}
};


}