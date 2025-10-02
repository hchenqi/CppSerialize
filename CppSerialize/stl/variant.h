#pragma once

#include "../layout_traits.h"

#include <variant>


namespace CppSerialize {


template<class T>
struct layout_traits<std::variant<T>> {
	constexpr static layout_size size() { return layout_traits<T>::size(); }
	constexpr static void read(auto f, const auto& object) { f(std::get<T>(object)); }
	constexpr static void write(auto f, auto& object) { f(std::get<T>(object)); }
};

template<class... Ts> requires (sizeof...(Ts) > 1)
struct layout_traits<std::variant<Ts...>> {
private:
	constexpr static bool equal(auto x, auto... xs) {
		return ((x == xs) && ...);
	}
	template<size_t I>
	constexpr static std::variant<Ts...> load_variant(size_t index, auto f) {
		if constexpr (I < sizeof...(Ts)) {
			if (index == I) {
				std::variant_alternative_t<I, std::variant<Ts...>> item;
				f(item);
				return item;
			}
			return load_variant<I + 1>(index, f);
		}
		throw std::runtime_error("invalid variant index");
	}
public:
	constexpr static layout_size size() {
		if constexpr (equal(layout_traits<Ts>::size()...)) {
			return layout_size{ sizeof(size_t) } + std::max({ layout_traits<Ts>::size()... });
		} else {
			return layout_size_dynamic;
		}
	}
	constexpr static void read(auto f, const auto& object) {
		f(object.index()); std::visit([&](auto& item) { f(item); }, object);
	}
	constexpr static void write(auto f, auto& object) {
		size_t index; f(index); object = load_variant<0>(index, f);
	}
};


}