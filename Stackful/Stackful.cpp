// Stackful.cpp : Defines the entry point for the console application.
//

#if defined(_WIN32) || defined(_WIN64)
#include "stdafx.h"
#endif
#include <assert.h>
#include <iostream>
#include <map>
#include <regex>
#include <stack>
#include <string>

#include "include/sfatoms.hpp"
#include "include/sfbuiltins.hpp"
#include "include/sftypes.hpp"
#include "include/sfextypes.hpp"

SFList tolist(const std::string &str) {
	SFList l;
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
	SFList ops;
	SFLiteral_p value = SFLiteral_p(new SFInteger(getAtom("nil")));
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

std::regex functionArityRegex(R"~(([a-z][a-z0-9_]+)\/?(\*|\d+)$)~");
SFFunctionArity_t getFunctionArity(const std::string &name, const int np) {
	std::sregex_iterator iter(name.begin(), name.end(), functionArityRegex);
	std::sregex_iterator end;
	if (iter == end) {
		return SFFunctionArity_t(name, np);
	}

	auto fnName = (*iter)[1];
	auto fnArity = (*iter)[2];
	if(fnArity == "*")
		return SFFunctionArity_t(fnName, '*');
	return SFFunctionArity_t(fnName, std::stoi(fnArity));
}

std::string fastr(const SFFunctionArity_t fa) {
	std::stringstream s;
	s << std::get<0>(fa);
	s << "/";
	char arity = std::get<1>(fa);
	if (arity == '*')
		s << "*";
	else
		s << (int)arity;
	return s.str();
}

void test() {
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

	//SFState state;

	std::cout << l4.str() << std::endl;
	std::cout << "String test: " << sfvar("Testing ABC").str() << std::endl;

	SFList str1 = tolist("Foobar whee");
	SFList str2 = tolist("Foobar whee");
	SFList str3 = tolist("Var humbug");

	std::cout << "String test 1: " << tostring(str1 == str2) << std::endl;
	std::cout << "String test 2: " << tostring(str1 == str3) << std::endl;

	SFClosure c;
	SFList *vKey = new SFList(tolist("Test"));
	SFList_p pKey(vKey);
	SFLiteral_p pValue(new SFList(str2));
	SFLiteral_p pValue2(new SFList(str3));
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
	printParams.push_back(sfvar("Test"));
	printParams.push_back(sfvar((SFInteger_t)1));
	printParams.push_back(sfvar((SFInteger_t)2));
	printParams.push_back(sfvar((SFInteger_t)3));
	print(printParams, SFClosure_p(new SFClosure(c)));

	std::string fa1 = "foo";
	std::string fa2 = "foo/*";
	std::string fa3 = "foo/2";

	std::cout << "FA1: " << fa1 << " = " << fastr(getFunctionArity(fa1, 2)) << std::endl;
	std::cout << "FA2: " << fa2 << " = " << fastr(getFunctionArity(fa2, 0)) << std::endl;
	std::cout << "FA2: " << fa3 << " = " << fastr(getFunctionArity(fa3, 0)) << std::endl;
}

int main()
{
	test();
    return 0;
}

