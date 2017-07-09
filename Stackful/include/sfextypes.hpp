#pragma once

// Extended types

#include "sftypes.hpp"

class SFXOpChain;
class SFXClosure;

typedef std::shared_ptr<SFXOpChain> SFOpChain_p;
typedef std::shared_ptr<SFXClosure> SFClosure_p;

enum ExtendedType {
	Atom,
	Integer,
	Float,
	String,
	List,
	FunctionCall,
	OpChain,
	Closure
};

class SFExtended : public SFList {
public:
	ExtendedType getExtendedType() const {
		return extendedType;
	}
	std::string str() const override {
		return _str();
	}
	std::string extStr() const {
		return _str();
	}
	virtual std::string extLiteral() const = 0;
	SFExtended(const SFExtended &copy) : SFList(copy), extendedType(copy.getExtendedType()) {
	}
protected:
	SFExtended(ExtendedType extType) : SFList(), extendedType(extType) {
	}
	SFExtended(ExtendedType extType, const SFList &copy) : SFList(copy), extendedType(extType) {
	}
	const ExtendedType extendedType;
	virtual std::string _str() const = 0;
};

class SFXAtom : public SFExtended {
public:
	SFXAtom(const std::string &name) : SFExtended(Atom) {
		push_back(Atom);
		push_back(getAtom(name));
	}
	std::string extLiteral() const {
		return "'" + getAtom(operator[](1).get()->IntegerClass()->getValue()) + "'";
	}
protected:
	std::string _str() const {
		return getAtom(operator[](1).get()->IntegerClass()->getValue());
	}
};

class SFXInteger : public SFExtended {
public:
	SFXInteger(const SFInteger_t val) : SFExtended(Integer) {
		push_back(Integer);
		push_back(val);
	}
	std::string extLiteral() const {
		return str();
	}
protected:
	std::string _str() const {
		return operator[](1).get()->str();
	}
};

class SFXFloat : public SFExtended {
public:
	union doublebits {
		double valueDouble;
		SFInteger_t valueInteger[sizeof(double) / sizeof(SFInteger_t)];
	};
	SFXFloat(double value) : SFExtended(Float) {
		push_back(Float);
		doublebits b;
		b.valueDouble = value;
		SFList *l = new SFList();
		for (auto i = 0; i < sizeof(double) / sizeof(SFInteger_t); i++) {
			l->push_back(b.valueInteger[i]);
		}
		push_back(l);
	}
	std::string extLiteral() const {
		return str();
	}
protected:
	std::string _str() const {
		doublebits b;
		const SFList &v = operator[](1).get()->ListClass();
		for (size_t i = 0; i < v.size(); i++) {
			b.valueInteger[i] = v[i].get()->IntegerClass()->getValue();
		}
		return std::to_string(b.valueDouble);
	}
};

class SFXString : public SFExtended {
public:
	SFXString(const std::string &str) : SFExtended(String) {
		SFList *l = new SFList();
		std::string::const_iterator it = str.begin();
		for (; it != str.end(); it++) {
			// Generally a char
			auto curr = *it;
			l->push_back(curr);
		}
		push_back(String);
		push_back(l);
	}
	std::string extLiteral() const {
		return "\"" + str() + "\"";
	}
protected:
	std::string _str() const {
		std::stringstream s;

		const SFList &v = operator[](1).get()->ListClass();
		for (size_t i = 0; i < v.size(); i++) {
			char c = (char)v[i].get()->IntegerClass()->getValue();
			s << c;
		}
		return s.str();
	}
};

class SFXList : public SFExtended {
public:
	SFXList() : SFExtended(List) {
	}
	SFXList(const SFXList &copy) : SFExtended(List, copy) {
	}
	std::string extLiteral() const {
		return str();
	}
protected:
	std::string _str() const {
		std::stringstream s;
		s << "[";
		bool first = true;
		auto it = begin();
		for (; it != end(); it++) {
			if (first)
				first = false;
			else
				s << ", ";
			s << it->get()->str();
		}
		s << "]";
		return s.str();
	}
};

// A closure, which is a list of sublists, which make up a key/value store.
// Each item in the list is composed of:
//		[key, value]
//	Where key is a list of integers representing a string.
//	And value is an SFLiteral_p
class SFXClosure : public SFExtended {
public:
	// Empty constructor with no parent
	SFXClosure() : SFExtended(Closure), parent(nullptr), topmost(nullptr) {
	}
	// Copy constructor
	SFXClosure(const SFXClosure &copy) : SFExtended(Closure, copy), parent(copy.getParent()), topmost(copy.getTopmost()) {
	}
	// Construct with ops and no parent
	SFXClosure(const SFList &ops) : SFExtended(Closure, ops), parent(nullptr), topmost(nullptr) {
	}
	// Construct with no ops and given parent (use shared ptr)
	SFXClosure(const SFClosure_p &parent) : SFExtended(Closure), parent(parent), topmost(parent.get()->getTopmost()) {
	}
	// Construct with ops and given parent (use shared ptr)
	SFXClosure(const SFClosure_p &parent, const SFList &ops) : SFExtended(Closure, ops), parent(parent), topmost(parent.get()->getTopmost()) {
	}
	~SFXClosure() {
	}
	SFClosure_p getParent() const { return parent; }
	SFClosure_p getTopmost() const {
		return topmost;
	}
	void setParent(const SFClosure_p &parent) {
		this->parent = parent;
		this->topmost = parent.get()->getTopmost();
	}
	SFLiteral_p get(const SFList_p &key) const throw(std::runtime_error) {
		return get(key.get());
	}
	SFLiteral_p get(const SFList &key) const throw(std::runtime_error) {
		SFList_t::iterator find = getByKey(key);
		if (find == end())
			throw std::runtime_error("Key not found: " + key.str());
		const SFList &list = find->get()->ListClass();
		return list[1];
	}
	void set(const SFList &key, const SFLiteral_p &value) {
		if (trySet(key, value))
			return;
		setImmediate(key, value);
	}
	void setImmediate(const SFList &key, const SFLiteral_p &value) {
		SFList_t::iterator find = getByKey(key);
		SFList *newKeyValue = new SFList();
		newKeyValue->push_back(key);
		newKeyValue->push_back(value);
		SFLiteral_p newKeyValue_p(newKeyValue);
		if (find == end()) {
			this->push_back(newKeyValue_p);
		} else {
			find->swap(newKeyValue_p);
		}
	}
	std::string extLiteral() const {
		return str();
	}
protected:
	SFClosure_p parent;
	SFClosure_p topmost;
	SFList_t::iterator getByKey(const SFList_p &key) const {
		return getByKey(key.get());
	}

	SFList_t::iterator getByKey(const SFList &key) const {
		SFList_t::iterator find = this->value->begin();
		for (; find != end(); find++) {
			// [key, value]
			const SFList &list = find->get()->ListClass();
			// check key
			if (list[0].get()->ListClass()->isEqual(&key))
				break;
		}
		return find;
	}
	bool hasKey(const SFList &key) const {
		return getByKey(key) != end();
	}
	bool trySet(const SFList &key, const SFLiteral_p &value) {
		if (hasKey(key)) {
			setImmediate(key, value);
			return true;
		}
		if (parent.get() != nullptr && parent.get()->trySet(key, value))
			return true;
		return false;
	}
protected:
	std::string _str() const {
		return "CLOSURE";
	}
};

// An OpChain, containing a Closure and operations to run.
class SFXOpChain : public SFExtended {
public:
	// An empty OpChain
	SFXOpChain() : SFExtended(Closure), parent(nullptr), closure(new SFXClosure()) {
	}
	// Copy constructor
	SFXOpChain(const SFXOpChain &copy) : SFExtended(Closure, copy), parent(copy.getParent()), closure(copy.getClosure()) {
	}
	// Initialize with given parent
	SFXOpChain(const SFOpChain_p &parent) : SFExtended(Closure), parent(parent), closure(new SFXClosure()) {
	}
	// Initialize with given parent and ops
	SFXOpChain(const SFOpChain_p &parent, const SFList_p &ops) : SFExtended(Closure, ops.get()), parent(parent), closure(parent.get()->getClosure()) {
	}
	SFOpChain_p getParent() const { return parent; };
	SFClosure_p getClosure() const { return closure; }
	std::string extLiteral() const {
		return str();
	}
protected:
	std::string str() const {
		return "OPCHAIN";
	}
	SFOpChain_p parent;
	SFClosure_p closure;
};

SFList sfvar(const std::string &str);
SFList sfvarfloat(const double value);
SFList sfvar(const SFInteger_t value);
SFList sfatom(const std::string &s);
ExtendedType identifyLiteral(const SFList &l);
std::string varstr(const SFList &l);

class SFXFunctionCall : public SFExtended {
public:
	SFXFunctionCall(const SFXFunctionCall &copy) : SFExtended(FunctionCall, copy) {
	}
	SFXFunctionCall(const std::string &fn);
	SFXFunctionCall(const std::string &fn, SFLiteral_p p1);
	SFXFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2);
	SFXFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2, SFLiteral_p p3);
	SFXFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2, SFLiteral_p p3, SFLiteral_p p4);
	std::string extLiteral() const {
		return str();
	}
protected:
	std::string _str() const {
		return "FunctionCall";
	}
};
