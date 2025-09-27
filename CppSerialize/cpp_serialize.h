#pragma once

#include "context.h"

#include <vector>


namespace CppSerialize {


constexpr size_t Size(const auto& object) {
	SizeContext size_context;
	size_context.add(object);
	return size_context.GetSize();
}

inline std::vector<byte> Serialize(const auto& object) {
	std::vector<byte> data(Size(object));
	SaveContext save_context(data.data(), data.size());
	save_context.save(object);
	return data;
}

inline void Deserialize(const std::vector<byte>& data, auto& object) {
	LoadContext load_context(data.data(), data.size());
	load_context.load(object);
}

template<class T>
inline T Deserialize(const std::vector<byte>& data) {
	T object;
	Deserialize(data, object);
	return object;
}


}