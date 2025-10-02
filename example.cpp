#include "CppSerialize/serializer.h"
#include "CppSerialize/stl/string.h"
#include "CppSerialize/stl/vector.h"
#include "CppSerialize/stl/array.h"
#include "CppSerialize/stl/variant.h"
#include "CppSerialize/stl/optional.h"

#include <cassert>


using namespace CppSerialize;


struct Empty {};
static_assert(layout_empty<Empty>);

struct Trivial {
	int i;
	double d;
};
static_assert(std::is_trivially_copyable_v<Trivial>);
static_assert(layout_trivial<Trivial>);

struct Custom {
	int i;
	double d;
};
constexpr auto layout(layout_type<Custom>) { return declare(&Custom::i, &Custom::d); }
static_assert(layout_custom<Custom>);

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
static_assert(layout_custom<Custom2>);

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
static_assert(layout_custom<Object>);

class Derived : public Object {
public:
	using layout_base = Object;
	using Object::Object;
private:
	void f() {}
};
static_assert(!layout_custom<Derived> && layout_derived<Derived>);

class Derived2 : public Derived {
public:
	using layout_base = Derived;
public:
	Derived2() = default;
	Derived2(int i, double j) : Derived(i), j(j) {}
private:
	double j = 0;
private:
	friend constexpr auto layout(layout_type<Derived2>) { return declare(&Derived2::j); }
};
static_assert(layout_custom<Derived2> && layout_derived<Derived2>);

class DerivedTrivial : public Trivial {
public:
	using layout_base = Trivial;
private:
	std::string s;
private:
	friend constexpr auto layout(layout_type<DerivedTrivial>) { return declare(&DerivedTrivial::s); }
};
static_assert(!std::is_trivially_copyable_v<DerivedTrivial>);
static_assert(layout_custom<DerivedTrivial> && layout_derived<DerivedTrivial>);


template<layout_size size>
constexpr void test(auto object) {
	auto data = Serialize(object).Get();
	static_assert(size == layout_traits<decltype(object)>::size());
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
	test<layout_size(4)>(Derived(1));
	test<layout_size(12)>(Derived2(1, 1.5));
	test<layout_size_dynamic>(DerivedTrivial());

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
