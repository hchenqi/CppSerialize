#pragma once

#include "layout_traits.h"

#include <string>
#include <vector>
#include <array>
#include <variant>
#include <optional>


namespace CppSerialize {


template<class T> requires has_trivial_layout<T>
struct layout_traits<std::basic_string<T>> {
	static void read(auto f, const auto& object) {
		f(object.size()); f(object.data(), object.size());
	}
	static void write(auto f, auto& object) {
		size_t count; f(count); object.resize(count); f(object.data(), count);
	}
};


template<class T> requires has_trivial_layout<T>
struct layout_traits<std::vector<T>> {
	static void read(auto f, const auto& object) {
		f(object.size()); f(object.data(), object.size());
	}
	static void write(auto f, auto& object) {
		size_t count; f(count); object.resize(count); f(object.data(), count);
	}
};

template<class T>
struct layout_traits<std::vector<T>> {
	static void read(auto f, const auto& object) {
		f(object.size()); for (auto& item : object) { f(item); }
	}
	static void write(auto f, auto& object) {
		size_t count; f(count); object.resize(count); for (auto& item : object) { f(item); }
	}
};


template<class T, size_t count> requires has_trivial_layout<T>
struct layout_traits<std::array<T, count>> {
	static void read(auto f, const auto& object) {
		f(object.data(), count);
	}
	static void write(auto f, auto& object) {
		f(object.data(), count);
	}
};

template<class T, size_t count>
struct layout_traits<std::array<T, count>> {
	static void read(auto f, const auto& object) {
		for (auto& item : object) { f(item); }
	}
	static void write(auto f, auto& object) {
		for (auto& item : object) { f(item); }
	}
};


template<class... Ts>
struct layout_traits<std::variant<Ts...>> {
private:
	template<size_t I>
	static std::variant<Ts...> load_variant(size_t index, auto f) {
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
	static void read(auto f, const auto& object) {
		f(object.index()); std::visit([&](auto& item) { f(item); }, object);
	}
	static void write(auto f, auto& object) {
		size_t index; f(index); object = load_variant<0>(index, f);
	}
};


template<class T>
struct layout_traits<std::optional<T>> {
	static void read(auto f, const auto& object) {
		f(object.has_value()); if (object.has_value()) { f(object.value()); }
	}
	static void write(auto f, auto& object) {
		bool has_value; f(has_value); if (has_value) { object = T(); f(object.value()); }
	}
};


}