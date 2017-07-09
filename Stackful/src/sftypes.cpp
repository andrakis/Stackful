#include <iostream>
#include "../include/sftypes.hpp"

SFLiteral::~SFLiteral() {
	return;
	std::cout << "A ";
	if (this->type == Basic_Integer) std::cout << "number " << this->IntegerClass()->getValue();
	else if (this->type == Basic_List) std::cout << "list";
	std::cout << " has died" << std::endl;
}

SFInteger* SFLiteral::IntegerClass () {
	if (this->type != Basic_Integer)
		throw std::runtime_error("Not a number");
	return static_cast<SFInteger*>(this);
}
const SFInteger* SFLiteral::IntegerClass () const {
	if (this->type != Basic_Integer)
		throw std::runtime_error("Not a number");
	return static_cast<const SFInteger*>(this);
}
SFList* SFLiteral::ListClass() {
	if (this->type != Basic_List)
		throw std::runtime_error("Not a list");
	return static_cast<SFList*>(this);
}
const SFList* SFLiteral::ListClass() const {
	if (this->type != Basic_List)
		throw std::runtime_error("Not a list");
	return static_cast<const SFList*>(this);
}

SFInteger operator + (const SFInteger &a, const SFInteger &b) {
	return SFInteger(a.getValue() + b.getValue());
}
SFInteger operator - (const SFInteger &a, const SFInteger &b) {
	return SFInteger(a.getValue() - b.getValue());
}
SFInteger operator * (const SFInteger &a, const SFInteger &b) {
	return SFInteger(a.getValue() * b.getValue());
}
SFInteger operator / (const SFInteger &a, const SFInteger &b) {
	return SFInteger(a.getValue() / b.getValue());
}
SFInteger operator | (const SFInteger &a, const SFInteger &b) {
	return SFInteger(a.getValue() | b.getValue());
}
SFInteger operator & (const SFInteger &a, const SFInteger &b) {
	return SFInteger(a.getValue() & b.getValue());
}
SFInteger operator ^ (const SFInteger &a, const SFInteger &b) {
	return SFInteger(a.getValue() ^ b.getValue());
}
SFInteger operator ! (const SFInteger &a) {
	return SFInteger(!a.getValue());
}
SFInteger operator << (const SFInteger &a, const SFInteger &b) {
	return SFInteger(a.getValue() << b.getValue());
}
SFInteger operator >> (const SFInteger &a, const SFInteger &b) {
	return SFInteger(a.getValue() >> b.getValue());
}

SFList operator + (const SFList &a, const SFList &b) {
	SFList result(a);
	result.ShallowCopy(&b);
	return result;
}

