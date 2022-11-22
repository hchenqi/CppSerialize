#include "layout_traits.h"

#include <vector>


namespace CppSerialize {


template<class T>
inline std::vector<byte> Save(const T& object) {
	SizeContext size_context;
	Size(size_context, object);
	std::vector<byte> data(size_context.GetSize());
	SaveContext save_context(data.data(), data.size());
	Save(save_context, object);
	return data;
}

template<class T>
inline void Load(const std::vector<byte>& data, T& object) {
	LoadContext load_context(data.data(), data.size());
	Load(load_context, object);
}

template<class T>
inline T Load(const std::vector<byte>& data) {
	T object;
	Load(data, object);
	return object;
}


}