#pragma once

#include "layout_traits.h"

#include <string>
#include <vector>
#include <array>
#include <variant>
#include <optional>


namespace CppSerialize {


template<class T> requires is_layout_trivial<T>
struct layout_traits<std::basic_string<T>> {
	constexpr static layout_size size() {
		return layout_size_dynamic;
	}
	constexpr static void read(auto f, const auto& object) {
		f(object.size()); f(object.data(), object.size());
	}
	constexpr static void write(auto f, auto& object) {
		size_t count; f(count); object.resize(count); f(object.data(), count);
	}
};


template<class T> requires is_layout_trivial<T>
struct layout_traits<std::vector<T>> {
	constexpr static layout_size size() {
		return layout_size_dynamic;
	}
	constexpr static void read(auto f, const auto& object) {
		f(object.size()); f(object.data(), object.size());
	}
	constexpr static void write(auto f, auto& object) {
		size_t count; f(count); object.resize(count); f(object.data(), count);
	}
};

template<class T>
struct layout_traits<std::vector<T>> {
	constexpr static layout_size size() {
		return layout_size_dynamic;
	}
	constexpr static void read(auto f, const auto& object) {
		f(object.size()); for (auto& item : object) { f(item); }
	}
	constexpr static void write(auto f, auto& object) {
		size_t count; f(count); object.resize(count); for (auto& item : object) { f(item); }
	}
};


template<class T>
struct layout_traits<std::array<T, 0>> {
	constexpr static layout_size size() { return layout_size_empty; }
	constexpr static void read(auto f, const auto& object) {}
	constexpr static void write(auto f, auto& object) {}
};

template<class T, size_t count> requires is_layout_trivial<T>
struct layout_traits<std::array<T, count>> {
	constexpr static layout_size size() {
		return count * layout_traits<T>::size();
	}
	constexpr static void read(auto f, const auto& object) {
		f(object.data(), count);
	}
	constexpr static void write(auto f, auto& object) {
		f(object.data(), count);
	}
};

template<class T, size_t count>
struct layout_traits<std::array<T, count>> {
	constexpr static layout_size size() {
		return count * layout_traits<T>::size();
	}
	constexpr static void read(auto f, const auto& object) {
		for (auto& item : object) { f(item); }
	}
	constexpr static void write(auto f, auto& object) {
		for (auto& item : object) { f(item); }
	}
};


template<class T>
struct layout_traits<std::variant<T>> {
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

template<class... Ts> requires (sizeof...(Ts) > 1)
struct layout_traits<std::variant<Ts...>> {
private:
	constexpr static bool equal(auto x, auto... xs) {
		return ((x == xs) && ...);
	}
	template<size_t I>
	constexpr static std::variant<Ts...> load_variant(size_t index, auto f) {
		if constexpr (I < sizeof...(Ts)) {
			if (index == I) {
				std::variant_alternative_t<I, std::variant<Ts...>> item;
				f(item);
				return item;
			}
			return load_variant<I + 1>(index, f);
		}
		throw std::runtime_error("invalid variant index");
	}
public:
	constexpr static layout_size size() {
		if constexpr (equal(layout_traits<Ts>::size()...)) {
			return layout_size{ sizeof(size_t) } + std::max({ layout_traits<Ts>::size()... });
		} else {
			return layout_size_dynamic;
		}
	}
	constexpr static void read(auto f, const auto& object) {
		f(object.index()); std::visit([&](auto& item) { f(item); }, object);
	}
	constexpr static void write(auto f, auto& object) {
		size_t index; f(index); object = load_variant<0>(index, f);
	}
};


template<class T>
struct layout_traits<std::optional<T>> {
	constexpr static layout_size size() {
		if constexpr (layout_traits<T>::size() == layout_size_empty) {
			return { sizeof(bool) };
		} else {
			return layout_size_dynamic;
		}
	}
	constexpr static void read(auto f, const auto& object) {
		f(object.has_value()); if (object.has_value()) { f(object.value()); }
	}
	constexpr static void write(auto f, auto& object) {
		bool has_value; f(has_value); if (has_value) { object = T(); f(object.value()); }
	}
};


}