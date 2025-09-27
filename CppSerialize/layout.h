#pragma once

#include <tuple>


namespace CppSerialize {


template<class T>
struct layout_type {};

template<class T>
constexpr auto layout(layout_type<T>) {}

template<class T, class... Ts>
constexpr auto declare(Ts T::*... member_list) { return std::make_tuple(member_list...); }

template<class T, class... Ts>
constexpr auto member_type_tuple(std::tuple<Ts T::*...>) -> std::tuple<Ts...> { return {}; }


template<class T>
constexpr bool is_layout_empty = std::is_empty_v<T>;

template<class T, class = void>
constexpr bool is_layout_custom = false;

template<class T>
constexpr bool is_layout_custom<T, decltype(member_type_tuple<T>(layout(layout_type<T>())), void())> = true;

template<class T>
constexpr bool is_layout_trivial = std::is_trivial_v<T> && !is_layout_empty<T> && !is_layout_custom<T>;


}