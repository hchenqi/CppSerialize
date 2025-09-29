#pragma once

#include "layout_traits.h"

#include <vector>
#include <array>
#include <bit>
#include <stdexcept>


namespace CppSerialize {


template<class T>
struct Size {
public:
	constexpr Size(const T& object) : object(object) {}
private:
	const T& object;
public:
	constexpr size_t Get() const {
		size_t size = 0;
		access(size, object);
		return size;
	}
private:
	constexpr static void access(size_t& size, const layout_static auto& object) {
		size += layout_size(layout_type<std::remove_cvref_t<decltype(object)>>());
	}
	constexpr static void access(size_t& size, const auto& object) {
		layout_traits<std::remove_cvref_t<decltype(object)>>::read([&](const auto& item) { access(size, item); }, object);
	}
};


template<class T>
struct Serialize {
public:
	constexpr Serialize(const T& object) : object(object) {}
private:
	const T& object;
public:
	constexpr std::vector<std::byte> Get() const {
		std::vector<std::byte> data; data.reserve(Size(object).Get());
		access(data, object);
		return data;
	}
private:
	constexpr static void access(std::vector<std::byte>& data, const layout_trivial auto& object) {
		auto bytes = std::bit_cast<std::array<std::byte, sizeof(object)>>(object);
		data.insert(data.end(), bytes.begin(), bytes.end());
	}
	constexpr static void access(std::vector<std::byte>& data, const auto& object) {
		layout_traits<std::remove_cvref_t<decltype(object)>>::read([&](const auto& item) { access(data, item); }, object);
	}
};


template<class T>
struct Deserialize {
public:
	constexpr Deserialize(const std::vector<std::byte>& data) : data(data) {}
private:
	const std::vector<std::byte>& data;
public:
	constexpr T Get() const {
		T object;
		std::vector<std::byte>::const_iterator index = data.begin();
		access(data, index, object);
		return object;
	}
private:
	constexpr static void access(const std::vector<std::byte>& data, std::vector<std::byte>::const_iterator& index, layout_trivial auto& object) {
		if (data.end() < index + sizeof(object)) {
			throw std::runtime_error("deserialization error");
		}
		std::array<std::byte, sizeof(object)> bytes;
		std::copy(index, index + sizeof(object), bytes.begin());
		object = std::bit_cast<std::remove_cvref_t<decltype(object)>>(bytes);
		index += sizeof(object);
	}
	constexpr static void access(const std::vector<std::byte>& data, std::vector<std::byte>::const_iterator& index, auto& object) {
		layout_traits<std::remove_cvref_t<decltype(object)>>::write([&](auto& item) { access(data, index, item); }, object);
	}
};


}