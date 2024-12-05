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
	template<class Func>
	static void Read(Func func, const std::basic_string<T>& object) {
		func(object.size()); func(object.data(), object.size());
	}
	template<class Func>
	static void Write(Func func, std::basic_string<T>& object) {
		size_t count; func(count); object.resize(count); func(object.data(), count);
	}
};


template<class T> requires has_trivial_layout<T>
struct layout_traits<std::vector<T>> {
	template<class Func>
	static void Read(Func func, const std::vector<T>& object) {
		func(object.size()); func(object.data(), object.size());
	}
	template<class Func>
	static void Write(Func func, std::vector<T>& object) {
		size_t count; func(count); object.resize(count); func(object.data(), count);
	}
};

template<class T>
struct layout_traits<std::vector<T>> {
	template<class Func>
	static void Read(Func func, const std::vector<T>& object) {
		func(object.size()); for (auto& item : object) { func(item); }
	}
	template<class Func>
	static void Write(Func func, std::vector<T>& object) {
		size_t count; func(count); object.resize(count); for (auto& item : object) { func(item); }
	}
};


template<class T, size_t count> requires has_trivial_layout<T>
struct layout_traits<std::array<T, count>> {
	template<class Func>
	static void Read(Func func, const std::array<T, count>& object) {
		func(object.data(), count);
	}
	template<class Func>
	static void Write(Func func, std::array<T, count>& object) {
		func(object.data(), count);
	}
};

template<class T, size_t count>
struct layout_traits<std::array<T, count>> {
	template<class Func>
	static void Read(Func func, const std::array<T, count>& object) {
		for (auto& item : object) { func(item); }
	}
	template<class Func>
	static void Write(Func func, std::array<T, count>& object) {
		for (auto& item : object) { func(item); }
	}
};


template<class... Ts>
struct layout_traits<std::variant<Ts...>> {
private:
	template<size_t I, class Func>
	static std::variant<Ts...> load_variant(size_t index, Func func) {
		if constexpr (I < sizeof...(Ts)) {
			if (index == I) {
				std::variant_alternative_t<I, std::variant<Ts...>> item;
				func(item);
				return item;
			}
			return load_variant<I + 1>(index, func);
		}
		throw std::runtime_error("invalid variant index");
	}
public:
	template<class Func>
	static void Read(Func func, const std::variant<Ts...>& object) {
		func(object.index()); std::visit([&](auto& item) { func(item); }, object);
	}
	template<class Func>
	static void Write(Func func, std::variant<Ts...>& object) {
		size_t index; func(index); object = load_variant<0>(index, func);
	}
};


template<class T>
struct layout_traits<std::optional<T>> {
	template<class Func>
	static void Read(Func func, const std::optional<T>& object) {
		func(object.has_value()); if (object.has_value()) { func(object.value()); }
	}
	template<class Func>
	static void Write(Func func, std::optional<T>& object) {
		bool has_value; func(has_value); if (has_value) { object = T(); func(object.value()); }
	}
};


}