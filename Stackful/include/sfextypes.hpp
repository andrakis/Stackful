#pragma once

// Extended types

#include "sftypes.hpp"

class SFOpChain;
class SFClosure;

typedef std::shared_ptr<SFOpChain> SFOpChain_p;
typedef std::shared_ptr<SFClosure> SFClosure_p;

enum ExtendedType {
	Atom,
	Number,
	Float,
	String,
	OpChain
};

// A closure, which is a list of sublists, which make up a key/value store.
// Each item in the list is composed of:
//		[key, value]
//	Where key is a list of integers representing a string.
//	And value is an SFLiteral_p
class SFClosure : public SFList {
public:
	// Empty constructor with no parent
	SFClosure() : SFList(), parent(nullptr), topmost(nullptr) {
	}
	// Copy constructor
	SFClosure(const SFClosure &copy) : SFList(copy), parent(copy.getParent()), topmost(copy.getTopmost()) {
	}
	// Construct with ops and no parent
	SFClosure(const SFList &ops) : SFList(ops), parent(nullptr), topmost(nullptr) {
	}
	// Construct with no ops and given parent (use shared ptr)
	SFClosure(const SFClosure_p &parent) : SFList(), parent(parent), topmost(parent.get()->getTopmost()) {
	}
	// Construct with ops and given parent (use shared ptr)
	SFClosure(const SFClosure_p &parent, const SFList &ops) : SFList(ops), parent(parent), topmost(parent.get()->getTopmost()) {
	}
	~SFClosure() {
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
};

// An OpChain, containing a Closure and operations to run.
class SFOpChain : public SFList {
public:
	// An empty OpChain
	SFOpChain() : SFList(), parent(nullptr), closure(new SFClosure()) {
	}
	// Copy constructor
	SFOpChain(const SFOpChain &copy) : SFList(copy), parent(copy.getParent()), closure(copy.getClosure()) {
	}
	// Initialize with given parent
	SFOpChain(const SFOpChain_p &parent) : SFList(), parent(parent), closure(new SFClosure()) {
	}
	// Initialize with given parent and ops
	SFOpChain(const SFOpChain_p &parent, const SFList_p &ops) : SFList(ops.get()), parent(parent), closure(parent.get()->getClosure()) {
	}
	SFOpChain_p getParent() const { return parent; };
	SFClosure_p getClosure() const { return closure; }
protected:
	SFOpChain_p parent;
	SFClosure_p closure;
};

SFList sfvar(const std::string &str);
SFList sfvar(const double value);
SFList sfvar(const SFInteger_t value);
SFList sfatom(const std::string &s);
ExtendedType identifyLiteral(const SFList &l);
std::string varstr(const SFList &l);

