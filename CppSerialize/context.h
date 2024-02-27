#pragma once

#include "layout_traits.h"

#include <stdexcept>


namespace CppSerialize {

using byte = std::byte;


template<class T>
constexpr void align_offset(size_t& offset) {
	constexpr size_t alignment = sizeof(T) <= 8 ? sizeof(T) : 8;  // 1, 2, 4, 8
	offset = (offset + (alignment - 1)) & ~(alignment - 1);
}

template<class T>
constexpr void align_offset(const byte*& data) {
	size_t offset = data - (byte*)nullptr; align_offset<T>(offset); data = (byte*)nullptr + offset;
}

template<class T>
constexpr void align_offset(byte*& data) {
	size_t offset = data - (byte*)nullptr; align_offset<T>(offset); data = (byte*)nullptr + offset;
}


struct SizeContext {
protected:
	size_t size;
public:
	SizeContext() : size(0) {}
public:
	template<class T> requires has_trivial_layout<T>
	void add(const T&) {
		align_offset<T>(size);
		size += sizeof(T);
	}
	template<class T> requires has_trivial_layout<T>
	void add(const T[], size_t count) {
		align_offset<T>(size);
		size += sizeof(T) * count;
	}
	template<class T>
	void add(const T& obj) {
		Read([&](auto&& ...args) { add(std::forward<decltype(args)>(args)...); }, obj);
	}
public:
	size_t GetSize() const { return size; }
};


struct SaveContext {
protected:
	byte* curr;
	byte* end;
public:
	SaveContext(byte* begin, size_t length) : curr(begin), end(begin + length) {}
protected:
	void CheckOffset(const byte* offset) { if (offset > end) { throw std::runtime_error("save error"); } }
public:
	template<class T> requires has_trivial_layout<T>
	void save(const T& object) {
		align_offset<T>(curr); byte* next = curr + sizeof(T); CheckOffset(next);
		memcpy(curr, &object, sizeof(T)); curr = next;
	}
	template<class T> requires has_trivial_layout<T>
	void save(const T object[], size_t count) {
		align_offset<T>(curr); byte* next = curr + sizeof(T) * count; CheckOffset(next);
		memcpy(curr, object, sizeof(T) * count); curr = next;
	}
	template<class T>
	void save(const T& obj) {
		Read([&](auto&& ...args) { save(std::forward<decltype(args)>(args)...); }, obj);
	}
};


struct LoadContext {
protected:
	const byte* curr;
	const byte* end;
public:
	LoadContext(const byte* begin, size_t length) : curr(begin), end(begin + length) {}
protected:
	void CheckOffset(const byte* offset) { if (offset > end) { throw std::runtime_error("load error"); } }
public:
	template<class T> requires has_trivial_layout<T>
	void load(T& object) {
		align_offset<T>(curr); const byte* next = curr + sizeof(T); CheckOffset(next);
		memcpy(&object, curr, sizeof(T)); curr = next;
	}
	template<class T> requires has_trivial_layout<T>
	void load(T object[], size_t count) {
		align_offset<T>(curr); const byte* next = curr + sizeof(T) * count; CheckOffset(next);
		memcpy(object, curr, sizeof(T) * count); curr = next;
	}
	template<class T>
	void load(T& obj) {
		Write([&](auto&& ...args) { load(std::forward<decltype(args)>(args)...); }, obj);
	}
};


}