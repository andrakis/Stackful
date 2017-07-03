// Stackful.cpp : Defines the entry point for the console application.
//

#if defined(_WIN32) || defined(_WIN64)
#include "stdafx.h"
#endif
#include <assert.h>
#include <iostream>
#include <map>
#include <stack>

#include "include/sfatoms.hpp"
#include "include/sftypes.hpp"

enum ExtendedType {
	Atom,
	Number,
	Float,
	String,
	OpChain
};

class SFState {
public:
	SFState() {
	}
	virtual ~SFState() {
		std::cout << "A state has died" << std::endl;
	}
	ExtendedType identifyLiteral(const SFList &l) {
		if (l.size() != 2)
			throw std::runtime_error(std::string("Invalid literal: ") + l.str());
		SFInteger *firstElement = l[0]->IntegerClass();
		return (ExtendedType)firstElement->getValue();
	}
protected:
	SFList ops;
	SFLiteral_p value = SFLiteral_p(new SFInteger(getAtom("nil")));
};

typedef std::shared_ptr<SFState> SFState_p;

// Specialized types
class SFOpChain;
class SFClosure;

typedef std::shared_ptr<SFOpChain> SFOpChain_p;
typedef std::shared_ptr<SFClosure> SFClosure_p;

// A closure, which is a list of sublists, which make up a key/value store.
// Each item in the list is composed of:
//		[key, value]
//	Where key is a list of integers representing a string.
//	And value is an SFLiteral_p
class SFClosure : public SFList {
public:
	// Empty constructor with no parent
	SFClosure() : SFList(), parent(nullptr) {
	}
	// Copy constructor
	SFClosure(const SFClosure &copy) : SFList(copy), parent(copy.getParent()) {
	}
	// Construct with ops and no parent
	SFClosure(const SFList &ops) : SFList(ops), parent(nullptr) {
	}
	// Construct with no ops and given parent (use shared ptr)
	SFClosure(const SFClosure_p &parent) : SFList(), parent(parent) {
	}
	// Construct with ops and given parent (use shared ptr)
	SFClosure(const SFClosure_p &parent, const SFList &ops) : SFList(ops), parent(parent) {
	}
	SFClosure_p getParent() const { return parent; }
	void setParent(const SFClosure_p &parent) {
		this->parent = parent;
	}
protected:
	SFClosure_p parent;
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

class SFInterpreter {
public:
	SFInterpreter() {
		push(new SFState());
	}
	~SFInterpreter() {
		while (false == stateStack.empty())
			stateStack.pop();
	}
	SFState_p top() const { return stateStack.top(); }
	void push(SFState *s) {
		stateStack.emplace(SFState_p(s));
	}
protected:
	std::stack<SFState_p> stateStack;
};

std::string tostring(bool value) {
	return value ? "true" : "false";
}

int main()
{
	SFInteger i1(1);
	SFInteger i2(5);
	SFInteger i3(i1 ^ i2);
	SFList l1;

	l1.push_back(i1);
	l1.push_back(i2);
	l1.push_back(i3);

	SFList l2(l1);
	SFList l3(l1 + l2);
	SFList l4(l1);

	assert((i1 == i2) == false);
	assert((i1 == SFInteger(1)) == true);
	assert((l1 == l2) == true);
	assert((l2 == l3) == false);

	l3.pop_back();
	std::cout << l3.str() << std::endl;

	l4.push_back(l1);

	SFState state;

	std::cout << l4.str() << std::endl;
    return 0;
}

