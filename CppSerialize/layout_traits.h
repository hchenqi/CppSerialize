#pragma once

#include "layout_size.h"


namespace CppSerialize {


template<class T>
concept layout_empty = std::is_empty_v<T>;


template<class T>
struct layout_traits {
	static_assert(std::is_trivially_copyable_v<T>, "object layout unspecified");
	struct trivial {};
};

template<class T>
concept layout_trivial = requires { typename layout_traits<T>::trivial; };


template<class T, class = void>
struct layout_custom_trait : std::false_type {};

template<class T>
struct layout_custom_trait<T, std::void_t<decltype(member_type_tuple<T>(layout(layout_type<T>())))>> : std::true_type {};

template<class T>
concept layout_custom = layout_custom_trait<T>::value;


template<class T>
constexpr layout_size::layout_size(layout_type<T>) {
	if constexpr (layout_trivial<T>) {
		size = sizeof(T);
	} else {
		size = layout_traits<T>::size();
	}
}

template<class T>
concept layout_static = layout_size(layout_type<T>()) != layout_size_dynamic;


template<class T> requires layout_empty<T>
struct layout_traits<T> {
	constexpr static layout_size size() { return layout_size_empty; }
	constexpr static void read(auto f, const auto& object) {}
	constexpr static void write(auto f, auto& object) {}
};


template<class T> requires layout_custom<T>
struct layout_traits<T> {
	constexpr static layout_size size() {
		return layout_size(layout_type<decltype(member_type_tuple<T>(layout(layout_type<T>())))>());
	}
	constexpr static void read(auto f, const auto& object) {
		std::apply([&](auto... member) { (f(object.*member), ...); }, layout(layout_type<T>()));
	}
	constexpr static void write(auto f, auto& object) {
		std::apply([&](auto... member) { (f(object.*member), ...); }, layout(layout_type<T>()));
	}
};


template<class T1, class T2>
struct layout_traits<std::pair<T1, T2>> {
	constexpr static layout_size size() {
		return layout_size(layout_type<T1>()) + layout_size(layout_type<T2>());
	}
	constexpr static void read(auto f, const auto& object) {
		f(object.first); f(object.second);
	}
	constexpr static void write(auto f, auto& object) {
		f(object.first); f(object.second);
	}
};


template<>
struct layout_traits<std::tuple<>> {
	constexpr static layout_size size() { return layout_size_empty; }
	constexpr static void read(auto f, const auto& object) {}
	constexpr static void write(auto f, auto& object) {}
};

template<class T>
struct layout_traits<std::tuple<T>> {
	constexpr static layout_size size() { return layout_size(layout_type<T>()); }
	constexpr static void read(auto f, const auto& object) { f(std::get<T>(object)); }
	constexpr static void write(auto f, auto& object) { f(std::get<T>(object)); }
};

template<class... Ts>
struct layout_traits<std::tuple<Ts...>> {
	constexpr static layout_size size() {
		return (layout_size(layout_type<Ts>()) + ...);
	}
	constexpr static void read(auto f, const auto& object) {
		std::apply([&](auto&... member) { (f(member), ...); }, object);
	}
	constexpr static void write(auto f, auto& object) {
		std::apply([&](auto&... member) { (f(member), ...); }, object);
	}
};


}