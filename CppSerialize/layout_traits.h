#pragma once

#include "context.h"


namespace CppSerialize {


template<class T>
struct layout_traits {
	static_assert((layout_type<T>(), false), "object layout unspecified");
};

template<class T> inline void Size(SizeContext& context, const T& object) { layout_traits<T>::Size(context, object); }
template<class T> inline void Save(SaveContext& context, const T& object) { layout_traits<T>::Save(context, object); }
template<class T> inline void Load(LoadContext& context, T& object) { layout_traits<T>::Load(context, object); }


template<class T> requires has_trivial_layout<T>
struct layout_traits<T> {
	static void Size(SizeContext& context, const T& object) { context.add(object); }
	static void Save(SaveContext& context, const T& object) { context.write(object); }
	static void Load(LoadContext& context, T& object) { context.read(object); }
};

template<class T> requires has_custom_layout<T>
struct layout_traits<T> {
	static void Size(SizeContext& context, const T& object) {
		std::apply([&](auto... member) { (CppSerialize::Size(context, object.*member), ...); }, layout(layout_type<T>()));
	}
	static void Save(SaveContext& context, const T& object) {
		std::apply([&](auto... member) { (CppSerialize::Save(context, object.*member), ...); }, layout(layout_type<T>()));
	}
	static void Load(LoadContext& context, T& object) {
		std::apply([&](auto... member) { (CppSerialize::Load(context, object.*member), ...); }, layout(layout_type<T>()));
	}
};


template<class T1, class T2>
struct layout_traits<std::pair<T1, T2>> {
	static void Size(SizeContext& context, const std::pair<T1, T2>& object) {
		CppSerialize::Size(context, object.first); CppSerialize::Size(context, object.second);
	}
	static void Save(SaveContext& context, const std::pair<T1, T2>& object) {
		CppSerialize::Save(context, object.first); CppSerialize::Save(context, object.second);
	}
	static void Load(LoadContext& context, std::pair<T1, T2>& object) {
		CppSerialize::Load(context, object.first); CppSerialize::Load(context, object.second);
	}
};

template<class... Ts>
struct layout_traits<std::tuple<Ts...>> {
	static void Size(SizeContext& context, const std::tuple<Ts...>& object) {
		std::apply([&](auto&... member) { (CppSerialize::Size(context, member), ...); }, object);
	}
	static void Save(SaveContext& context, const std::tuple<Ts...>& object) {
		std::apply([&](auto&... member) { (CppSerialize::Save(context, member), ...); }, object);
	}
	static void Load(LoadContext& context, std::tuple<Ts...>& object) {
		std::apply([&](auto&... member) { (CppSerialize::Load(context, member), ...); }, object);
	}
};


}