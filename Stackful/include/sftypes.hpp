#pragma once

#include <exception>
#include <inttypes.h>	// uint32_t
#include <iostream>
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
	const SFInteger *IntegerClass() const;
	const SFList *ListClass() const;

	virtual std::string str() const = 0;

	bool operator == (const SFLiteral &other) const {
		return equals(other);
	}

	bool equals(const SFLiteral &other) const {
		if (this->getType() != other.getType())
			return false;
		return _isequal(other);
	}

protected:
	SFLiteral(const SFType t) : type(t) {
	}

	virtual bool _isequal(const SFLiteral &other) const = 0;

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
	bool _isequal(const SFLiteral &other) const {
		return literalEquals(other);
	}
	virtual bool literalEquals(const SFLiteral &other) const = 0;
	T value;
};

// Generic number class
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
protected:
	bool literalEquals(const SFLiteral &other) const {
		return value == other.IntegerClass()->getValue();
	}
};

SFInteger operator + (const SFInteger &a, const SFInteger &b);
SFInteger operator - (const SFInteger &a, const SFInteger &b);
SFInteger operator * (const SFInteger &a, const SFInteger &b);
SFInteger operator / (const SFInteger &a, const SFInteger &b);
SFInteger operator | (const SFInteger &a, const SFInteger &b);
SFInteger operator & (const SFInteger &a, const SFInteger &b);
SFInteger operator ^ (const SFInteger &a, const SFInteger &b);
SFInteger operator ! (const SFInteger &a);
SFInteger operator << (const SFInteger &a, const SFInteger &b);
SFInteger operator >> (const SFInteger &a, const SFInteger &b);

typedef std::shared_ptr<SFLiteral> SFLiteral_p;
typedef std::vector<SFLiteral_p> SFList_t;

// A list that may contain numbers or other lists
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
			value->push_back(*it);
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
		if (size() == 0)
			std::cout << "List::clear() called when empty!" << std::endl;
		value->clear();
	}
	SFLiteral_p pop_back() {
		if (size() == 0)
			throw std::runtime_error("pop_back(): List is empty");
		SFLiteral_p p = value->at(size() - 1);
		value->pop_back();
		return p;
	}
	void push_back(SFLiteral_p p) {
		value->push_back(p);
	}
	void push_back(SFLiteral *v) {
		value->push_back(SFLiteral_p(v));
	}
	void push_back(const SFInteger i) {
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
	size_t size() const {
		return value->size();
	}
	SFList_t::iterator begin() const { return value->begin(); }
	SFList_t::iterator end() const { return value->end(); }
	SFLiteral_p operator[](SFList_t::size_type _Pos) const {
		return at(_Pos);
	}
	SFLiteral_p at(const SFList_t::size_type _Pos) const {
		return value->at(_Pos);
	}
	void set(const SFList_t::size_type _Pos, const SFLiteral_p &v) {
		value->at(_Pos) = v;
	}
	bool isEqual(const SFList *other_list) const {
		if (size() != other_list->size())
			return false;
		SFList_t::iterator our_it = begin();
		SFList_t::iterator other_it = other_list->begin();
		for (; our_it != end(), other_it != other_list->end(); our_it++, other_it++) {
			const SFLiteral *our_item = our_it->get();
			const SFLiteral *other_item = other_it->get();
			if (our_item->equals(*other_item) == false)
				return false;
		}
		return true;
	}
protected:
	// Compare every item in the list. May result in recursive calls.
	bool literalEquals(const SFLiteral &other) const {
		const SFList *other_list = other.ListClass();
		return isEqual(other_list);
	}
};

typedef std::shared_ptr<SFList> SFList_p;

SFList operator + (const SFList &a, const SFList &b);

