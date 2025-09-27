#pragma once

#include "layout.h"
#include "layout_size.h"


namespace CppSerialize {


template<class T>
struct layout_traits {
	static_assert((std::void_t<T>(), false), "object layout unspecified");
};


template<class T> requires is_layout_empty<T>
struct layout_traits<T> {
	constexpr static layout_size size() { return layout_size_empty; }
	constexpr static void read(auto f, const auto& object) {}
	constexpr static void write(auto f, auto& object) {}
};

template<class T> requires is_layout_trivial<T>
struct layout_traits<T> {
	constexpr static layout_size size() { return { sizeof(T) }; }
};


template<class T> requires is_layout_custom<T>
struct layout_traits<T> {
	constexpr static layout_size size() {
		return layout_traits<decltype(member_type_tuple<T>(layout(layout_type<T>())))>::size();
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
		return layout_traits<T1>::size() + layout_traits<T2>::size();
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
	constexpr static layout_size size() {
		return layout_traits<T>::size();
	}
	constexpr static void read(auto f, const auto& object) {
		f(std::get<T>(object));
	}
	constexpr static void write(auto f, auto& object) {
		f(std::get<T>(object));
	}
};

template<class... Ts>
struct layout_traits<std::tuple<Ts...>> {
	constexpr static layout_size size() {
		return (layout_traits<Ts>::size() + ...);
	}
	constexpr static void read(auto f, const auto& object) {
		std::apply([&](auto&... member) { (f(member), ...); }, object);
	}
	constexpr static void write(auto f, auto& object) {
		std::apply([&](auto&... member) { (f(member), ...); }, object);
	}
};


}