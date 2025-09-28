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


template<class T, class = void>
struct layout_custom_trait : std::false_type {};

template<class T>
struct layout_custom_trait<T, std::void_t<decltype(member_type_tuple<T>(layout(layout_type<T>())))>> : std::true_type {};


template<class T>
concept layout_custom = layout_custom_trait<T>::value;

template<class T>
concept layout_empty = std::is_empty_v<T>;

template<class T>
concept layout_trivial = std::is_trivially_copyable_v<T> && !layout_empty<T> && !layout_custom<T>;


}