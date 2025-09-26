#pragma once

#include "layout.h"


namespace CppSerialize {


template<class T>
struct layout_traits {
	static_assert((std::void_t<T>(), false), "object layout unspecified");
};


template<class T> requires has_custom_layout<T>
struct layout_traits<T> {
	static void read(auto f, const auto& object) {
		std::apply([&](auto... member) { (f(object.*member), ...); }, layout(layout_type<T>()));
	}
	static void write(auto f, auto& object) {
		std::apply([&](auto... member) { (f(object.*member), ...); }, layout(layout_type<T>()));
	}
};


template<class T1, class T2>
struct layout_traits<std::pair<T1, T2>> {
	static void read(auto f, const auto& object) {
		f(object.first); f(object.second);
	}
	static void write(auto f, auto& object) {
		f(object.first); f(object.second);
	}
};


template<class... Ts>
struct layout_traits<std::tuple<Ts...>> {
	static void read(auto f, const auto& object) {
		std::apply([&](auto&... member) { (f(member), ...); }, object);
	}
	static void write(auto f, auto& object) {
		std::apply([&](auto&... member) { (f(member), ...); }, object);
	}
};


}