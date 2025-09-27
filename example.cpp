#include "CppSerialize/cpp_serialize.h"
#include "CppSerialize/layout_traits_stl.h"

#include <cassert>


using namespace CppSerialize;


struct Empty {};

struct Trivial {
	int i;
	double d;
};

struct Custom {
	int i;
	double d = 0.0;
	std::string s;
	std::vector<Custom> v;
};
auto layout(layout_type<Custom>) {
	return declare(
		&Custom::s,
		&Custom::i,
		&Custom::v
	);
}

class Object {
public:
	Object() {}
	Object(int i) : i(i) {}
private:
	int i;
public:
	mutable double temporary = 0.0;
private:
	friend auto layout(layout_type<Object>) { return declare(&Object::i); }
};


template<layout_size size>
void test(auto object) {
	std::vector<byte> data = Serialize(object);
	static_assert(size == layout_traits<decltype(object)>::size());
	if constexpr (size != layout_size_dynamic) {
		assert(data.size() == size);
	}
	auto copy = Deserialize<decltype(object)>(data);
	auto copy_data = Serialize(copy);
	assert(data == copy_data);
}

int main() {
	test<layout_size_empty>(Empty{});
	test<layout_size(16)>(Trivial{ 1, 1.5 });
	test<layout_size_dynamic>(Custom{ 1, 1.5, "hello", { Custom{ 2, 2.5, "world"}} });
	test<layout_size(4)>(Object(1));

	test<layout_size(12)>(std::make_pair(1, 1.5));
	test<layout_size(4)>(std::make_pair(1, Empty{}));
	test<layout_size_empty>(std::make_tuple());
	test<layout_size(4)>(std::make_tuple(1));
	test<layout_size(12)>(std::make_tuple(1, 1.5, Empty{}));
	test<layout_size_dynamic>(std::make_tuple(1, 1.5, std::string("abc")));

	test<layout_size_dynamic>(std::string("abc"));

	test<layout_size_dynamic>(std::vector<int>{1, 2, 3});
	test<layout_size_dynamic>(std::vector<std::string>{ std::string("1"), "2", "3" });

	test<layout_size_empty>(std::array<std::string, 0>{});
	test<layout_size(12)>(std::array{ 1, 2, 3 });
	test<layout_size_dynamic>(std::array{ std::string("1"), std::string("2") });

	test<layout_size(4)>(std::variant<int>(1));
	test<layout_size_dynamic>(std::variant<int, double>(1));
	test<layout_size(16)>(std::variant<unsigned long long, double>(1.5));

	test<layout_size(1)>(std::optional<Empty>());
	test<layout_size_dynamic>(std::optional<int>(1));
	test<layout_size_dynamic>(std::optional<std::string>("abc"));
}
