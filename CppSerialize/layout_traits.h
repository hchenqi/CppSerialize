#pragma once

#include "layout.h"


namespace CppSerialize {


template<class T>
struct layout_traits {
	static_assert((std::void_t<T>(), false), "object layout unspecified");
};


template<class Func, class T>
inline void Read(Func func, const T& object) {
	layout_traits<T>::Read(func, object);
}
template<class Func, class T>
inline void Write(Func func, T& object) {
	layout_traits<T>::Write(func, object);
}


template<class T> requires has_custom_layout<T>
struct layout_traits<T> {
	template<class Func>
	static void Read(Func func, const T& object) {
		std::apply([&](auto... member) { (func(object.*member), ...); }, layout(layout_type<T>()));
	}
	template<class Func>
	static void Write(Func func, T& object) {
		std::apply([&](auto... member) { (func(object.*member), ...); }, layout(layout_type<T>()));
	}
};


template<class T1, class T2>
struct layout_traits<std::pair<T1, T2>> {
	template<class Func>
	static void Read(Func func, const std::pair<T1, T2>& object) {
		func(object.first); func(object.second);
	}
	template<class Func>
	static void Write(Func func, std::pair<T1, T2>& object) {
		func(object.first); func(object.second);
	}
};


template<class... Ts>
struct layout_traits<std::tuple<Ts...>> {
	template<class Func>
	static void Read(Func func, const std::tuple<Ts...>& object) {
		std::apply([&](auto&... member) { (func(member), ...); }, object);
	}
	template<class Func>
	static void Write(Func func, std::tuple<Ts...>& object) {
		std::apply([&](auto&... member) { (func(member), ...); }, object);
	}
};


}