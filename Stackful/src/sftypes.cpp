#include "../stdafx.h"
#include <iostream>
#include "../include/sftypes.hpp"

namespace stackful {

	SFLiteral::~SFLiteral() {
#ifdef PETTY_DEBUG
		debug << "A ";
		if (this->type == Basic_Integer) debug << "number " << this->IntegerClass()->getValue();
		else if (this->type == Basic_List) debug << "list";
		debug << " has died" << std::endl;
#endif
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
	SFBasicInteger operator % (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() % b.getValue());
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

#if 0
	SFLiteral* operator + (const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.getType() != b.getType())
			throw std::runtime_error("Basic type mismatch");
		switch (a.getType()) {
			case Basic_Integer:
			{
				const SFBasicInteger *ba = a.IntegerClass();
				const SFBasicInteger *bb = b.IntegerClass();
				return new SFBasicInteger(ba->getValue() + bb->getValue());
			}
			case Basic_List:
			{
				if (a.isExtended()) {
					const SFExtended *ba = a.ExtClass();
					const SFExtended *bb = b.ExtClass();
					return ba->add(bb);
				} else {
					const SFBasicList &ba = a.ListClass();
					const SFBasicList &bb = b.ListClass();
					return new SFBasicList(ba + bb);
				}
			}
		}
	}
#endif
}
