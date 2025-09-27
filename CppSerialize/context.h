#pragma once

#include "layout_traits.h"

#include <stdexcept>


namespace CppSerialize {

using byte = std::byte;


struct SizeContext {
protected:
	size_t size;
public:
	SizeContext() : size(0) {}
public:
	template<class T> requires has_trivial_layout<T>
	void add(const T&) {
		size += sizeof(T);
	}
	template<class T> requires has_trivial_layout<T>
	void add(const T[], size_t count) {
		size += sizeof(T) * count;
	}
	template<class T>
	void add(const T& obj) {
		layout_traits<T>::read([&](auto&& ...args) { add(std::forward<decltype(args)>(args)...); }, obj);
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
		byte* next = curr + sizeof(T); CheckOffset(next);
		memcpy(curr, &object, sizeof(T)); curr = next;
	}
	template<class T> requires has_trivial_layout<T>
	void save(const T object[], size_t count) {
		byte* next = curr + sizeof(T) * count; CheckOffset(next);
		memcpy(curr, object, sizeof(T) * count); curr = next;
	}
	template<class T>
	void save(const T& obj) {
		layout_traits<T>::read([&](auto&& ...args) { save(std::forward<decltype(args)>(args)...); }, obj);
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
		const byte* next = curr + sizeof(T); CheckOffset(next);
		memcpy(&object, curr, sizeof(T)); curr = next;
	}
	template<class T> requires has_trivial_layout<T>
	void load(T object[], size_t count) {
		const byte* next = curr + sizeof(T) * count; CheckOffset(next);
		memcpy(object, curr, sizeof(T) * count); curr = next;
	}
	template<class T>
	void load(T& obj) {
		layout_traits<T>::write([&](auto&& ...args) { load(std::forward<decltype(args)>(args)...); }, obj);
	}
};


}