#pragma once

#include "layout.h"

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
private:
	size_t size;
public:
	SizeContext() : size(0) {}
public:
	template<class T> requires has_trivial_layout<T>
	void add(const T&) {
		align_offset<T>(size);
		size += sizeof(T);
	}
	template<class T>requires has_trivial_layout<T>
	void add(T[], size_t count) {
		align_offset<T>(size);
		size += sizeof(T) * count;
	}
public:
	size_t GetSize() const { return size; }
};


struct SaveContext {
private:
	byte* curr;
	byte* end;
public:
	SaveContext(byte* begin, size_t length) : curr(begin), end(begin + length) {}
private:
	void CheckOffset(const byte* offset) { if (offset > end) { throw std::runtime_error("save error"); } }
public:
	template<class T> requires has_trivial_layout<T>
	void write(const T& object) {
		align_offset<T>(curr); byte* next = curr + sizeof(T); CheckOffset(next);
		memcpy(curr, &object, sizeof(T)); curr = next;
	}
	template<class T> requires has_trivial_layout<T>
	void write(const T object[], size_t count) {
		align_offset<T>(curr); byte* next = curr + sizeof(T) * count; CheckOffset(next);
		memcpy(curr, object, sizeof(T) * count); curr = next;
	}
};


struct LoadContext {
private:
	const byte* curr;
	const byte* end;
public:
	LoadContext(const byte* begin, size_t length) : curr(begin), end(begin + length) {}
private:
	void CheckOffset(const byte* offset) { if (offset > end) { throw std::runtime_error("load error"); } }
public:
	template<class T> requires has_trivial_layout<T>
	void read(T& object) {
		align_offset<T>(curr); const byte* next = curr + sizeof(T); CheckOffset(next);
		memcpy(&object, curr, sizeof(T)); curr = next;
	}
	template<class T> requires has_trivial_layout<T>
	void read(T object[], size_t count) {
		align_offset<T>(curr); const byte* next = curr + sizeof(T) * count; CheckOffset(next);
		memcpy(object, curr, sizeof(T) * count); curr = next;
	}
};


}