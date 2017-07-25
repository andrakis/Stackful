#include <iostream>
#include "../include/sftypes.hpp"

namespace stackful {

	SFLiteral::~SFLiteral() {
		return;
		debug << "A ";
		if (this->type == Basic_Integer) debug << "number " << this->IntegerClass()->getValue();
		else if (this->type == Basic_List) debug << "list";
		debug << " has died" << std::endl;
	}

	SFBasicInteger* SFLiteral::IntegerClass() {
		if (this->type != Basic_Integer)
			throw std::runtime_error("Not a number");
		return static_cast<SFBasicInteger*>(this);
	}
	const SFBasicInteger* SFLiteral::IntegerClass() const {
		if (this->type != Basic_Integer)
			throw std::runtime_error("Not a number");
		return static_cast<const SFBasicInteger*>(this);
	}
	SFBasicList* SFLiteral::ListClass() {
		if (this->type != Basic_List)
			throw std::runtime_error("Not a list");
		return static_cast<SFBasicList*>(this);
	}
	const SFBasicList* SFLiteral::ListClass() const {
		if (this->type != Basic_List)
			throw std::runtime_error("Not a list");
		return static_cast<const SFBasicList*>(this);
	}

	SFBasicInteger operator + (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() + b.getValue());
	}
	SFBasicInteger operator - (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() - b.getValue());
	}
	SFBasicInteger operator * (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() * b.getValue());
	}
	SFBasicInteger operator / (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() / b.getValue());
	}
	SFBasicInteger operator | (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() | b.getValue());
	}
	SFBasicInteger operator & (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() & b.getValue());
	}
	SFBasicInteger operator ^ (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() ^ b.getValue());
	}
	SFBasicInteger operator ! (const SFBasicInteger &a) {
		return SFBasicInteger(!a.getValue());
	}
	SFBasicInteger operator << (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() << b.getValue());
	}
	SFBasicInteger operator >> (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() >> b.getValue());
	}

	SFBasicList operator + (const SFBasicList &a, const SFBasicList &b) {
		SFBasicList result(a);
		result.ShallowCopy(&b);
		return result;
	}
}
