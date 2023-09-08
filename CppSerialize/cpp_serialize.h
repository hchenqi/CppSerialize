#include "context.h"

#include <vector>


namespace CppSerialize {


template<class T>
inline std::vector<byte> Serialize(const T& object) {
	SizeContext size_context;
	size_context.add(object);
	std::vector<byte> data(size_context.GetSize());
	SaveContext save_context(data.data(), data.size());
	save_context.save(object);
	return data;
}

template<class T>
inline void Deserialize(const std::vector<byte>& data, T& object) {
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