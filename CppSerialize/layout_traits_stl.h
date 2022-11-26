#pragma once

#include "layout_traits.h"

#include <string>
#include <vector>
#include <array>
#include <variant>


namespace CppSerialize {


template<class T> requires has_trivial_layout<T>
struct layout_traits<std::basic_string<T>> {
	static void Size(SizeContext& context, const std::basic_string<T>& object) {
		context.add(object.size()); context.add(object.data(), object.size());
	}
	static void Save(SaveContext& context, const std::basic_string<T>& object) {
		context.write(object.size()); context.write(object.data(), object.size());
	}
	static void Load(LoadContext& context, std::basic_string<T>& object) {
		size_t count; context.read(count); object.resize(count); context.read(object.data(), count);
	}
};


template<class T> requires has_trivial_layout<T>
struct layout_traits<std::vector<T>> {
	static void Size(SizeContext& context, const std::vector<T>& object) {
		context.add(object.size()); context.add(object.data(), object.size());
	}
	static void Save(SaveContext& context, const std::vector<T>& object) {
		context.write(object.size()); context.write(object.data(), object.size());
	}
	static void Load(LoadContext& context, std::vector<T>& object) {
		size_t count; context.read(count); object.resize(count); context.read(object.data(), count);
	}
};

template<class T>
struct layout_traits<std::vector<T>> {
	static void Size(SizeContext& context, const std::vector<T>& object) {
		context.add(object.size());	for (auto& item : object) { CppSerialize::Size(context, item); }
	}
	static void Save(SaveContext& context, const std::vector<T>& object) {
		context.write(object.size()); for (auto& item : object) { CppSerialize::Save(context, item); }
	}
	static void Load(LoadContext& context, std::vector<T>& object) {
		size_t count; context.read(count); object.resize(count); for (auto& item : object) { CppSerialize::Load(context, item); }
	}
};


template<class T, size_t count> requires has_trivial_layout<T>
struct layout_traits<std::array<T, count>> {
	static void Size(SizeContext& context, const std::array<T, count>& object) {
		context.add(object.data(), count);
	}
	static void Save(SaveContext& context, const std::array<T, count>& object) {
		context.write(object.data(), count);
	}
	static void Load(LoadContext& context, std::array<T, count>& object) {
		context.read(object.data(), count);
	}
};

template<class T, size_t count>
struct layout_traits<std::array<T, count>> {
	static void Size(SizeContext& context, const std::array<T, count>& object) {
		for (auto& item : object) { CppSerialize::Size(context, item); }
	}
	static void Save(SaveContext& context, const std::array<T, count>& object) {
		for (auto& item : object) { CppSerialize::Save(context, item); }
	}
	static void Load(LoadContext& context, std::array<T, count>& object) {
		for (auto& item : object) { CppSerialize::Load(context, item); }
	}
};


template<class... Ts>
struct layout_traits<std::variant<Ts...>> {
private:
	template<size_t I>
	static std::variant<Ts...> load_variant(LoadContext& context, size_t index) {
		if constexpr (I < sizeof...(Ts)) {
			if (index == I) { std::variant_alternative_t<I, std::variant<Ts...>> item; CppSerialize::Load(context, item); return item; }
			return load_variant<I + 1>(context, index);
		}
		throw std::runtime_error("invalid variant index");
	}
public:
	static void Size(SizeContext& context, const std::variant<Ts...>& object) {
		context.add(object.index()); std::visit([&](auto& item) { CppSerialize::Size(context, item); }, object);
	}
	static void Save(SaveContext& context, const std::variant<Ts...>& object) {
		context.write(object.index()); std::visit([&](auto& item) { CppSerialize::Save(context, item); }, object);
	}
	static void Load(LoadContext& context, std::variant<Ts...>& object) {
		size_t index; context.read(index); object = load_variant<0>(context, index);
	}
};


}