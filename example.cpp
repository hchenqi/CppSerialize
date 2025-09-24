#include "CppSerialize/cpp_serialize.h"
#include "CppSerialize/layout_traits_stl.h"


using namespace CppSerialize;


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


int main() {
	std::vector<byte> data;

	data = Serialize(std::make_pair(1, 1.5));
	auto pair = Deserialize<std::pair<int, double>>(data);

	data = Serialize(std::vector<int>{1, 2, 3});
	auto vector = Deserialize<std::vector<int>>(data);

	data = Serialize(std::variant<int, double>(1));
	auto variant = Deserialize<std::variant<int, double>>(data);

	data = Serialize(std::optional<std::string>("abc"));
	auto optional = Deserialize<std::optional<std::string>>(data);

	data = Serialize(Trivial{ 1, 1.5 });
	auto trivial = Deserialize<Trivial>(data);

	data = Serialize(Custom{ 1, 1.5, "hello", { Custom{ 2, 2.5, "world"}} });
	auto custom = Deserialize<Custom>(data);

	data = Serialize(Object(1));
	auto object = Deserialize<Object>(data);
}
