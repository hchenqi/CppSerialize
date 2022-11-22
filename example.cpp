#include "cpp_serialize.h"
#include "layout_traits_stl.h"


using namespace CppSerialize;


struct Trivial {
	int i;
	double d;
};


struct Custom {
	Trivial t;
	std::string s;
	std::vector<Custom> v;
};

auto layout(layout_type<Custom>) {
	return declare(
		&Custom::t,
		&Custom::s,
		&Custom::v
	);
}


class Private {
public:
	Private() {}
	Private(Custom c) : c(c) {}
private:
	friend auto layout(layout_type<Private>);
	Custom c;
public:
	mutable int t = 0;  // not saved
};

auto layout(layout_type<Private>) { return declare(&Private::c); }


int main() {
	std::vector<byte> data;

	data = Save(Trivial{ 1, 1.5 });
	Trivial t = Load<Trivial>(data);

	data = Save(Custom{ t, "hello", { Custom{ Trivial{2, 0.5}, "world"}} });
	Custom c = Load<Custom>(data);

	data = Save(Private(c));
	Private p = Load<Private>(data);
}