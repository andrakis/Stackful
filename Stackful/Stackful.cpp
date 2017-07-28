// Stackful.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <assert.h>
#include <map>
#include <stack>
#include <string>

#include "include/sfatoms.hpp"
#include "include/sfbuiltins.hpp"
#include "include/sftypes.hpp"
#include "include/sfextypes.hpp"
#include "include/sfdebug.hpp"
#include "include/sfinterp.hpp"

using namespace stackful;

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
		debug << "A state has died" << std::endl;
	}
protected:
	SFBasicList ops;
	SFLiteral_p value = SFLiteral_p(new SFBasicInteger(getAtom("nil")));
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
	debug << l3.str() << std::endl;

	l4.push_back(l1);

	//SFState state;

	debug << l4.str() << std::endl;
	debug << "String test: " << sfvar("Testing ABC").str() << std::endl;

	SFBasicList str1 = xtolist("Foobar whee");
	SFBasicList str2 = xtolist("Foobar whee");
	SFBasicList str3 = xtolist("Var humbug");

	debug << "String test 1: " << tostring(str1 == str2) << std::endl;
	debug << "String test 2: " << tostring(str1 == str3) << std::endl;

	SFClosure c;
	SFBasicList *vKey = new SFBasicList(xtolist("Test"));
	SFList_p pKey(vKey);
	SFLiteral_p pValue(new SFBasicList(str2));
	SFLiteral_p pValue2(new SFBasicList(str3));
	c.set(vKey, pValue);
	debug << "Closure test: " << c.get(pKey)->str() << std::endl;
	// Ensure this only results in 1 item
	c.set(vKey, pValue2);
	assert(c.size() == 1);
	debug << "Closure test: " << c.get(pKey)->str() << std::endl;
#if defined(_DEBUG) && defined(_DEBUG_EXCEPTIONS)
	try {
		SFLiteral_p not_exist(c.get(str3));
		debug << "Closure test failure: " << not_exist << std::endl;
	} catch (std::runtime_error e) {
		debug << "Closure test OK" << std::endl;
	}
#endif

	stackful::setupBuiltins();
	SFBuiltin_f print = getBuiltin("print/*");
	SFList printParams;
	// "Test" 1 2 3
	printParams.push_back(SFLiteral_p(new SFString("Test")));
	printParams.push_back(SFLiteral_p(new SFInteger(1)));
	printParams.push_back(SFLiteral_p(new SFInteger(2)));
	printParams.push_back(SFLiteral_p(new SFInteger(3)));
	printParams.push_back(SFLiteral_p(new SFFloat(12.34)));
	debug << "(print " << printParams.extLiteral() << ")" << std::endl;
	SFInterpreter interp;
	SFBuiltinSignature_t params = { printParams, SFClosure_p(new SFClosure(c)), interp};
	print(params);

	std::string fa1 = "foo";
	std::string fa2 = "foo/*";
	std::string fa3 = "foo/2";

	debug << "FA1: " << fa1 << " = " << fastr(getFunctionArity(fa1, 2)) << std::endl;
	debug << "FA2: " << fa2 << " = " << fastr(getFunctionArity(fa2, 0)) << std::endl;
	debug << "FA2: " << fa3 << " = " << fastr(getFunctionArity(fa3, 0)) << std::endl;
}

int main()
{
	debug.setEnabled(true);
#if _DEBUG
	test();
#endif
	interp_test();
    return 0;
}

