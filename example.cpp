#include "CppSerialize/serializer.h"
#include "CppSerialize/stl/string.h"
#include "CppSerialize/stl/vector.h"
#include "CppSerialize/stl/array.h"
#include "CppSerialize/stl/variant.h"
#include "CppSerialize/stl/optional.h"

#include <cassert>


using namespace CppSerialize;


struct Empty {};

struct Trivial {
	int i;
	double d;
};

struct Custom {
	int i;
	double d;
};
constexpr auto layout(layout_type<Custom>) { return declare(&Custom::i, &Custom::d); }

struct Custom2 {
	int i;
	double d = 0.0;
	std::string s;
	std::vector<Custom2> v;
};
constexpr auto layout(layout_type<Custom2>) {
	return declare(
		&Custom2::s,
		&Custom2::i,
		&Custom2::v
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
	friend constexpr auto layout(layout_type<Object>) { return declare(&Object::i); }
};


template<layout_size size>
constexpr void test(auto object) {
	auto data = Serialize(object).Get();
	static_assert(size == layout_size(layout_type<decltype(object)>()));
	if constexpr (size != layout_size_dynamic) {
		static_assert(size == Size(object).Get());
		assert(data.size() == size);
	}
	auto copy = Deserialize<decltype(object)>(data).Get();
	auto data_copy = Serialize(copy).Get();
	assert(data == data_copy);
}

int main() {
	test<layout_size_empty>(Empty{});
	test<layout_size(16)>(Trivial{ 1, 1.5 });
	test<layout_size(12)>(Custom{ 1, 1.5 });
	test<layout_size_dynamic>(Custom2{ 1, 1.5, "hello", { Custom2{ 2, 2.5, "world"}} });
	test<layout_size(4)>(Object(1));

	test<layout_size(12)>(std::make_pair(1, 1.5));
	test<layout_size(4)>(std::make_pair(1, Empty{}));
	test<layout_size_empty>(std::make_pair(Empty{}, Empty{}));
	test<layout_size_empty>(std::make_tuple());
	test<layout_size_empty>(std::make_tuple(Empty{}));
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
	test<layout_size(8)>(std::variant<double>(1.5));
	test<layout_size_dynamic>(std::variant<int, double>(1));
	test<layout_size(16)>(std::variant<unsigned long long, double>(1.5));

	test<layout_size(1)>(std::optional<Empty>());
	test<layout_size_dynamic>(std::optional<int>(1));
	test<layout_size_dynamic>(std::optional<std::string>("abc"));
}
