// Stackful.cpp : Defines the entry point for the console application.
//

#if defined(_WIN32) || defined(_WIN64)
#include "stdafx.h"
#endif
#include <assert.h>
#include <iostream>
#include <map>
#include <stack>
#include <string>

#include "include/sfatoms.hpp"
#include "include/sfbuiltins.hpp"
#include "include/sftypes.hpp"
#include "include/sfextypes.hpp"

SFBasicList xtolist(const std::string &str) {
	SFBasicList l;
	std::string::const_iterator it = str.begin();
	for (; it != str.end(); it++) {
		// Generally a char
		auto curr = *it;
		l.push_back(curr);
	}
	return l;
}

class SFState {
public:
	SFState() {
	}
	virtual ~SFState() {
		std::cout << "A state has died" << std::endl;
	}
protected:
	SFBasicList ops;
	SFLiteral_p value = SFLiteral_p(new SFBasicInteger(getAtom("nil")));
};

typedef std::shared_ptr<SFState> SFState_p;

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

std::string tostring(const SFLiteral &l) {
	return l.str();
}

void test() {
	SFBasicInteger i1(1);
	SFBasicInteger i2(5);
	SFBasicInteger i3(i1 ^ i2);
	SFBasicList l1;

	l1.push_back(i1);
	l1.push_back(i2);
	l1.push_back(i3);

	SFBasicList l2(l1);
	SFBasicList l3(l1 + l2);
	SFBasicList l4(l1);

	assert((i1 == i2) == false);
	assert((i1 == SFBasicInteger(1)) == true);
	assert((l1 == l2) == true);
	assert((l2 == l3) == false);

	l3.pop_back();
	std::cout << l3.str() << std::endl;

	l4.push_back(l1);

	//SFState state;

	std::cout << l4.str() << std::endl;
	std::cout << "String test: " << sfvar("Testing ABC").str() << std::endl;

	SFBasicList str1 = xtolist("Foobar whee");
	SFBasicList str2 = xtolist("Foobar whee");
	SFBasicList str3 = xtolist("Var humbug");

	std::cout << "String test 1: " << tostring(str1 == str2) << std::endl;
	std::cout << "String test 2: " << tostring(str1 == str3) << std::endl;

	SFClosure c;
	SFBasicList *vKey = new SFBasicList(xtolist("Test"));
	SFList_p pKey(vKey);
	SFLiteral_p pValue(new SFBasicList(str2));
	SFLiteral_p pValue2(new SFBasicList(str3));
	c.set(vKey, pValue);
	std::cout << "Closure test: " << c.get(pKey)->str() << std::endl;
	// Ensure this only results in 1 item
	c.set(vKey, pValue2);
	assert(c.size() == 1);
	std::cout << "Closure test: " << c.get(pKey)->str() << std::endl;
#ifndef NDEBUG
	try {
		SFLiteral_p not_exist(c.get(str3));
		std::cout << "Closure test failure: " << not_exist << std::endl;
	} catch (std::runtime_error e) {
		std::cout << "Closure test OK" << std::endl;
	}
#endif

	setupBuiltins();
	SFBuiltin_f print = getBuiltin("print/*");
	SFList printParams;
	// "Test" 1 2 3
	printParams.push_back(SFLiteral_p(new SFString("Test")));
	printParams.push_back(SFLiteral_p(new SFInteger(1)));
	printParams.push_back(SFLiteral_p(new SFInteger(2)));
	printParams.push_back(SFLiteral_p(new SFInteger(3)));
	printParams.push_back(SFLiteral_p(new SFFloat(12.34)));
	print(printParams, SFClosure_p(new SFClosure(c)));

	std::string fa1 = "foo";
	std::string fa2 = "foo/*";
	std::string fa3 = "foo/2";

	std::cout << "FA1: " << fa1 << " = " << fastr(getFunctionArity(fa1, 2)) << std::endl;
	std::cout << "FA2: " << fa2 << " = " << fastr(getFunctionArity(fa2, 0)) << std::endl;
	std::cout << "FA2: " << fa3 << " = " << fastr(getFunctionArity(fa3, 0)) << std::endl;

	// Test var, get
	// ((var A "foobar")
	//  (print "A:" (get 'A')))
	SFOpChain ocTest;
	SFFunctionCall *fcVar = new SFFunctionCall("var", SFLiteral_p(new SFAtom("A")), SFLiteral_p(new SFString("foobar")));
	SFFunctionCall *fcGet = new SFFunctionCall("get", SFLiteral_p(new SFAtom("A")));
	SFFunctionCall *fcPrint = new SFFunctionCall("print", SFLiteral_p(new SFString("A:")), SFLiteral_p(fcGet));
	ocTest.push_back(SFLiteral_p(fcVar));
	ocTest.push_back(SFLiteral_p(fcPrint));

	std::cout << ocTest.str() << std::endl;
}

int main()
{
	test();
    return 0;
}

