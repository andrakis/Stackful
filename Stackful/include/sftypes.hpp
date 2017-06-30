#pragma once

#include <exception>
#include <inttypes.h>	// uint32_t
#include <memory>		// shared_ptr
#include <sstream>		// stringstream
#include <vector>

// The size of our basic number type
typedef uint32_t SFInteger_t;

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

	virtual std::string str() const = 0;

protected:
	SFLiteral(const SFType t) : type(t) {
	}

	const SFType type;
};

// The specialised value
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
	std::string str() const {
		std::stringstream ss;
		ss << value;
		return ss.str();
	}
};

SFInteger operator + (const SFInteger a, const SFInteger b);
SFInteger operator - (const SFInteger a, const SFInteger b);
SFInteger operator * (const SFInteger a, const SFInteger b);
SFInteger operator / (const SFInteger a, const SFInteger b);
SFInteger operator | (const SFInteger a, const SFInteger b);
SFInteger operator & (const SFInteger a, const SFInteger b);
SFInteger operator ^ (const SFInteger a, const SFInteger b);
SFInteger operator ! (const SFInteger a);
SFInteger operator << (const SFInteger a, const SFInteger b);
SFInteger operator >> (const SFInteger a, const SFInteger b);

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
		SFList_t::iterator it = l->begin();
		for (; it != l->end(); it++) {
			value->push_back(SFLiteral_p(*it));
		}
	}
	void ShallowCopy(const SFList &l) {
		// Pass to above function
		ShallowCopy(&l);
	}
	~SFList() {
		clear();
	}
	void clear() {
		value->clear();
	}
	void push_back(SFLiteral *v) {
		value->push_back(SFLiteral_p(v));
	}
	void push_back(SFInteger i) {
		push_back(new SFInteger(i));
	}
	void push_back(const SFList l) {
		SFList *n = new SFList(l);
		push_back(n);
		// n will be freed by the shared pointer
	}
	std::string str() const {
		std::stringstream ss;
		SFList_t::iterator it = begin();
		bool first = true;
		ss << "[";
		for (; it != end(); it++) {
			if (first)
				first = false;
			else
				ss << ", ";
			ss << it->get()->str();
		}
		ss << "]";
		return ss.str();
	}
	size_t size() {
		return value->size();
	}
	SFList_t::iterator begin() const { return value->begin(); }
	SFList_t::iterator end() const { return value->end(); }
};

typedef std::shared_ptr<SFList> SFList_p;

SFList operator + (const SFList &a, const SFList &b);
