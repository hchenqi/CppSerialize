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


}