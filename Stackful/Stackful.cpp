// Stackful.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <exception>
#include <inttypes.h>	// uint32_t
#include <iostream>
#include <map>
#include <memory>		// shared_ptr
#include <sstream>
#include <vector>

typedef uint32_t SFInteger_t;

// TODO: This should be a fixed size vector
typedef std::vector<std::string> SFAtomMap_t;
typedef std::map<SFInteger_t, std::string> SFStringMap_t;

// Forward declarations
class SFLiteral;
class SFInteger;
class SFList;

enum SFType {
	Integer,
	List
};

// The generic literal value
class SFLiteral {
public:
	virtual ~SFLiteral();
	SFType getType() const { return type; }

	SFInteger *IntegerClass();
	SFList *ListClass();

	virtual std::string str() = 0;

	// TODO:
	// std::string str () const { }
protected:
	SFLiteral(const SFType t) : type(t) {
	}

	const SFType type;
};

template<class T>
class SFLiteralT : public SFLiteral {
public:
	T getValue() const { return value; }
protected:
	SFLiteralT(const T v, SFType type) : value(v), SFLiteral(type) {
	}
	T value;
};

class SFInteger : public SFLiteralT<const SFInteger_t> {
public:
	SFInteger(const SFInteger_t v) : SFLiteralT(v, Integer) {
	}
	SFInteger(const SFInteger &v) : SFLiteralT(v.getValue(), Integer) {
	}
	std::string str() {
		std::stringstream ss;
		ss << value;
		return ss.str();
	}
};

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

typedef std::shared_ptr<SFLiteral> SFLiteral_p;
typedef std::vector<SFLiteral_p> SFList_t;

class SFList : public SFLiteralT<SFList_t*> {
public:
	SFList() : SFLiteralT(new SFList_t(), List) {
	}
	SFList(const SFList &l) : SFLiteralT(new SFList_t(), List) {
		ShallowCopy(l);
	}
	SFList(const SFList *l) : SFLiteralT(new SFList_t(), List) {
		ShallowCopy(l);
	}
	void ShallowCopy(const SFList *l) {
		SFList_t::iterator it = l->getValue()->begin();
		for (; it != l->getValue()->end(); it++) {
			this->value->push_back(SFLiteral_p(*it));
		}
	}
	void ShallowCopy(const SFList &l) {
		ShallowCopy(&l);
	}
	~SFList() {
		this->value->clear();
	}
	void push_back(SFLiteral *v) {
		this->value->push_back(SFLiteral_p(v));
	}
	void push_back(SFInteger i) {
		this->push_back(new SFInteger(i));
	}
	void push_back(const SFList l) {
		SFList *n = new SFList(l);
		push_back(n);
		// n will be freed by the shared pointer
	}
	std::string str() {
		std::stringstream ss;
		SFList_t::iterator it = value->begin();
		bool first = true;
		ss << "[";
		for (; it != value->end(); it++) {
			if (first)
				first = false;
			else
				ss << ", ";
			ss << it->get()->str();
		}
		ss << "]";
		return ss.str();
	}
};

typedef std::shared_ptr<SFList> SFList_p;

SFList operator + (const SFList &a, const SFList &b) {
	SFList result(a);
	result.ShallowCopy(&b);
	return result;
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

SFLiteral::~SFLiteral() {
	std::cout << "A ";
	if (this->type == Integer) std::cout << "number " << this->IntegerClass()->getValue();
	else if (this->type == List) std::cout << "list";
	std::cout << " has died" << std::endl;
}

class SFState {
public:
	SFState() {
		atomMap.push_back("nil");
		atomMap.push_back("false");
		atomMap.push_back("true");
	}
	virtual ~SFState() {
	}
protected:
	SFList ops;
	SFAtomMap_t atomMap;
	SFStringMap_t stringMap;
};

int main()
{
	SFInteger i1(1);
	SFInteger i2(2);
	SFInteger i3(i1 ^ i2);
	SFList l1;

	l1.push_back(i1);
	l1.push_back(i2);
	l1.push_back(i3);

	SFList l2(l1);
	SFList l3(l1 + l2);
	SFList l4(l1);

	l4.push_back(l1);

	SFState state;

	std::cout << l4.str() << std::endl;
    return 0;
}

