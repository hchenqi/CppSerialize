#pragma once

#include "layout_size.h"


namespace CppSerialize {


template<class T>
concept layout_empty = std::is_empty_v<T>;


template<class T>
struct layout_traits_trivial {
	constexpr static layout_size size() { return { sizeof(T) }; }
	constexpr static void read(auto f, const T& object) { f(object); }
	constexpr static void write(auto f, T& object) { f(object); }
};


template<class T>
struct layout_traits : layout_traits_trivial<T> {
	static_assert(std::is_trivially_copyable_v<T>, "object layout unspecified");
};

template<class T>
concept layout_trivial = std::is_base_of_v<layout_traits_trivial<T>, layout_traits<T>>;


template<class T, class = void>
struct layout_custom_trait : std::false_type {};

template<class T>
struct layout_custom_trait<T, std::void_t<decltype(member_type_tuple<T>(layout(layout_type<T>())))>> : std::true_type {};

template<class T>
concept layout_custom = layout_custom_trait<T>::value;


template<class T>
concept layout_derived = requires { typename T::layout_base; };


template<class T>
concept layout_static = layout_traits<T>::size() != layout_size_dynamic;

template<class T>
concept layout_dynamic = !layout_static<T>;


template<class T> requires layout_empty<T>
struct layout_traits<T> {
	constexpr static layout_size size() { return layout_size_empty; }
	constexpr static void read(auto f, const auto& object) {}
	constexpr static void write(auto f, auto& object) {}
};


template<class T> requires layout_custom<T>
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


template<class T> requires layout_derived<T>
struct layout_traits<T> : layout_traits<typename T::layout_base> {};


template<class T> requires layout_custom<T> && layout_derived<T>
struct layout_traits<T> : layout_traits<typename T::layout_base> {
	constexpr static layout_size size() {
		return layout_traits<typename T::layout_base>::size() + layout_traits<decltype(member_type_tuple<T>(layout(layout_type<T>())))>::size();
	}
	constexpr static void read(auto f, const auto& object) {
		layout_traits<typename T::layout_base>::read(f, object), std::apply([&](auto... member) { (f(object.*member), ...); }, layout(layout_type<T>()));
	}
	constexpr static void write(auto f, auto& object) {
		layout_traits<typename T::layout_base>::write(f, object), std::apply([&](auto... member) { (f(object.*member), ...); }, layout(layout_type<T>()));
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
	constexpr static layout_size size() { return layout_traits<T>::size(); }
	constexpr static void read(auto f, const auto& object) { f(std::get<T>(object)); }
	constexpr static void write(auto f, auto& object) { f(std::get<T>(object)); }
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