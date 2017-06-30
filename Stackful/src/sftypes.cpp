#include <iostream>
#include "../include/sftypes.hpp"

SFLiteral::~SFLiteral() {
	std::cout << "A ";
	if (this->type == Integer) std::cout << "number " << this->IntegerClass()->getValue();
	else if (this->type == List) std::cout << "list";
	std::cout << " has died" << std::endl;
}

SFInteger* SFLiteral::IntegerClass () {
	if (this->type != Integer)
		throw std::exception("Not a number");
	return static_cast<SFInteger*>(this);
}
SFList* SFLiteral::ListClass() {
	if (this->type != List)
		throw std::exception("Not a list");
	return static_cast<SFList*>(this);
}

SFInteger operator + (const SFInteger a, const SFInteger b) {
	return SFInteger(a.getValue() + b.getValue());
}
SFInteger operator - (const SFInteger a, const SFInteger b) {
	return SFInteger(a.getValue() - b.getValue());
}
SFInteger operator * (const SFInteger a, const SFInteger b) {
	return SFInteger(a.getValue() * b.getValue());
}
SFInteger operator / (const SFInteger a, const SFInteger b) {
	return SFInteger(a.getValue() / b.getValue());
}
SFInteger operator | (const SFInteger a, const SFInteger b) {
	return SFInteger(a.getValue() | b.getValue());
}
SFInteger operator & (const SFInteger a, const SFInteger b) {
	return SFInteger(a.getValue() & b.getValue());
}
SFInteger operator ^ (const SFInteger a, const SFInteger b) {
	return SFInteger(a.getValue() ^ b.getValue());
}
SFInteger operator ! (const SFInteger a) {
	return SFInteger(!a.getValue());
}
SFInteger operator << (const SFInteger a, const SFInteger b) {
	return SFInteger(a.getValue() << b.getValue());
}
SFInteger operator >> (const SFInteger a, const SFInteger b) {
	return SFInteger(a.getValue() >> b.getValue());
}

SFList operator + (const SFList &a, const SFList &b) {
	SFList result(a);
	result.ShallowCopy(&b);
	return result;
}
