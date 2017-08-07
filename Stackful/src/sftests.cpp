#include "../include/sfinterp.hpp"

#include <assert.h>
#include <chrono>
#include <map>
#include <stack>
#include <string>

using namespace stackful;

void test_factorial() {
	// Test a factorial function
	// ((def fac #N :: (
	//    (if (<= (get N) 1) (
	//       (1)
	//    ) (else (
	//       (* (get N) (fac (- (get N) 1)))
	//    )))
	//  ))
	//  (var X 10)
	//  (print "Fac of" (get X) (fac (get X)))
	// )
	SFOpChain *chain = new SFOpChain();

	auto start = std::chrono::steady_clock::now();

	setupBuiltins();
	chain->importClosure(getBuiltinDefinitions());
	SFLiteral_p chain_p(chain);

	auto step1 = std::chrono::steady_clock::now();
	std::cerr << "Builtins imported in " <<
		std::chrono::duration_cast<std::chrono::milliseconds>(step1 - start).count()
		<< "ms" << std::endl;

	SFOpChain *facBody = new SFOpChain(chain_p);
	SFLiteral_p facBody_p(facBody);
	SFFunctionCall *fcGetN = new SFFunctionCall("get", getAtomPtr("N"));
	SFLiteral_p fcGetN_p(fcGetN);
	SFFunctionCall *fcCompare = new SFFunctionCall("<=", fcGetN_p, SFLiteral_p(new SFInteger(1)));
	SFOpChain *facBodyIf1True = new SFOpChain(facBody_p);
	facBodyIf1True->push_back(SFLiteral_p(new SFInteger(1)));
	SFOpChain *facBodyIf1False = new SFOpChain(facBody_p);
	SFFunctionCall *fcFacBodyIf1False = new SFFunctionCall(
		"*", SFLiteral_p(new SFFunctionCall(
			"fac", SFLiteral_p(new SFFunctionCall(
			"-", fcGetN_p, SFLiteral_p(new SFInteger(1))
		)))), fcGetN_p
	);
	facBodyIf1False->push_back(SFLiteral_p(fcFacBodyIf1False));
	SFFunctionCall *fcIf = new SFFunctionCall(
		"if", SFLiteral_p(fcCompare), SFLiteral_p(facBodyIf1True),
		SFLiteral_p(new SFFunctionCall("else", SFLiteral_p(facBodyIf1False)))
	);
	facBody->push_back(SFLiteral_p(fcIf));
	SFFunctionDefinition *fnDef = new SFFunctionDefinition(chain_p, "fac", { "N" }, facBody_p);
	SFLiteral_p fnDef_p(fnDef);
	SFFunctionCall *fcDef = new SFFunctionCall("def", getAtomPtr("fac"), fnDef_p);
	chain->push_back(SFLiteral_p(fcDef));

#if !defined(_NO_PROMOTE) && !defined(_DEBUG)
	SFLiteral_p X(new SFInteger(50));
#else
	SFLiteral_p X(new SFFloat(10.0));
#endif
	SFFunctionCall *fcVarN = new SFFunctionCall("var", getAtomPtr("X"), X);
	chain->push_back(SFLiteral_p(fcVarN));

	SFFunctionCall *fcGetX = new SFFunctionCall("get", getAtomPtr("X"));
	SFLiteral_p fcGetX_p(fcGetX);
	SFFunctionCall *fcPrint = new SFFunctionCall("print/*",
		SFLiteral_p(new SFString("Fac of")), fcGetX_p,
		SFLiteral_p(new SFFunctionCall("fac", fcGetX_p))
	);
	chain->push_back(SFLiteral_p(fcPrint));
	debug << chain->str() << std::endl;

	SFInterpreter si;

	auto step2 = std::chrono::steady_clock::now();
	std::cerr << "Chain created in " <<
		std::chrono::duration_cast<std::chrono::milliseconds>(step2 - step1).count()
		<< "ms" << std::endl;
	SFLiteral_p result = si.run(chain_p);
	auto step3 = std::chrono::steady_clock::now();
	std::cerr << "Code run in " <<
		std::chrono::duration_cast<std::chrono::milliseconds>(step3 - step2).count()
		<< "ms" << std::endl;
	debug << "Result: " << result->str() << std::endl;
}

void test_fibonacci() {
	// Test a factorial function
	// ((def fib #N :: (
	//    (if (< N 2) (
	//      (1)
	//    ) (else (
	//      (+ (fib (- N 1)) (fib (- N 2)))
	//    )))
	//  ))
	//  (var X 10)
	//  (print "Fac of" (get X) (fac (get X)))
	// )
	std::cerr << "Starting fibonacci test" << std::endl;
	SFOpChain *chain = new SFOpChain();

	auto start = std::chrono::steady_clock::now();

	setupBuiltins();
	chain->importClosure(getBuiltinDefinitions());
	SFLiteral_p chain_p(chain);

	auto step1 = std::chrono::steady_clock::now();
	std::cerr << "Builtins imported in " <<
		std::chrono::duration_cast<std::chrono::milliseconds>(step1 - start).count()
		<< "ms" << std::endl;

	SFOpChain *facBody = new SFOpChain(chain_p);
	SFLiteral_p facBody_p(facBody);
	SFFunctionCall *fcGetN = new SFFunctionCall("get", getAtomPtr("N"));
	SFLiteral_p fcGetN_p(fcGetN);
	SFFunctionCall *fcCompare = new SFFunctionCall("<=", fcGetN_p, SFLiteral_p(new SFInteger(2)));
	SFOpChain *facBodyIf1True = new SFOpChain(facBody_p);
	facBodyIf1True->push_back(SFLiteral_p(new SFInteger(1)));
	SFOpChain *facBodyIf1False = new SFOpChain(facBody_p);
	SFFunctionCall *fcFacBodyIf1False = new SFFunctionCall(
		"+",
		SFLiteral_p(new SFFunctionCall(
			"fib", SFLiteral_p(new SFFunctionCall(
				"-", fcGetN_p, SFLiteral_p(new SFInteger(1))
				)))),
		SFLiteral_p(new SFFunctionCall(
			"fib", SFLiteral_p(new SFFunctionCall(
				"-", fcGetN_p, SFLiteral_p(new SFInteger(2))
				))))
	);
	facBodyIf1False->push_back(SFLiteral_p(fcFacBodyIf1False));
	SFFunctionCall *fcIf = new SFFunctionCall(
		"if", SFLiteral_p(fcCompare), SFLiteral_p(facBodyIf1True),
		SFLiteral_p(new SFFunctionCall("else", SFLiteral_p(facBodyIf1False)))
	);
	facBody->push_back(SFLiteral_p(fcIf));
	SFFunctionDefinition *fnDef = new SFFunctionDefinition(chain_p, "fib", { "N" }, facBody_p);
	SFLiteral_p fnDef_p(fnDef);
	SFFunctionCall *fcDef = new SFFunctionCall("def", getAtomPtr("fib"), fnDef_p);
	chain->push_back(SFLiteral_p(fcDef));

#if !defined(_NO_PROMOTE) && !defined(_DEBUG)
	SFLiteral_p X(new SFInteger(15));
#else
	SFLiteral_p X(new SFInteger(10));
#endif
	SFFunctionCall *fcVarN = new SFFunctionCall("var", getAtomPtr("X"), X);
	chain->push_back(SFLiteral_p(fcVarN));

	SFFunctionCall *fcGetX = new SFFunctionCall("get", getAtomPtr("X"));
	SFLiteral_p fcGetX_p(fcGetX);
	SFFunctionCall *fcPrint = new SFFunctionCall("print/*",
		SFLiteral_p(new SFString("Fib of")), fcGetX_p,
		SFLiteral_p(new SFFunctionCall("fib", fcGetX_p))
	);
	chain->push_back(SFLiteral_p(fcPrint));
	debug << chain->str() << std::endl;

	SFInterpreter si;

	auto step2 = std::chrono::steady_clock::now();
	std::cerr << "Chain created in " <<
		std::chrono::duration_cast<std::chrono::milliseconds>(step2 - step1).count()
		<< "ms" << std::endl;
	SFLiteral_p result = si.run(chain_p);
	auto step3 = std::chrono::steady_clock::now();
	std::cerr << "Code run in " <<
		std::chrono::duration_cast<std::chrono::milliseconds>(step3 - step2).count()
		<< "ms" << std::endl;
	debug << "Result: " << result->str() << std::endl;
}

void test_comparison() {
	// (
	//     (var A 12)
	//     (var B 2)
	//     (print "A < B" (< (get A) (get B)))
	// )
	SFOpChain *chain = new SFOpChain();
	SFLiteral_p chain_p(chain);

	chain->importClosure(getBuiltinDefinitions());

	SFFunctionCall *fcVarA = new SFFunctionCall("var", getAtomPtr("A"), SFLiteral_p(new SFInteger(12)));
	SFFunctionCall *fcVarB = new SFFunctionCall("var", getAtomPtr("B"), SFLiteral_p(new SFInteger(2)));
	SFFunctionCall *fcGetA = new SFFunctionCall("get", getAtomPtr("A"));
	SFFunctionCall *fcGetB = new SFFunctionCall("get", getAtomPtr("B"));
	SFLiteral_p pGetA(fcGetA);
	SFLiteral_p pGetB(fcGetB);
	SFFunctionCall *fcPrint1 = new SFFunctionCall("print",
		SFLiteral_p(new SFString("A == B:")), SFLiteral_p(
			new SFFunctionCall("==", pGetA, pGetB)
		));
	SFFunctionCall *fcPrint2 = new SFFunctionCall("print",
		SFLiteral_p(new SFString("A != B:")), SFLiteral_p(
			new SFFunctionCall("!=", pGetA, pGetB)
		));
	SFFunctionCall *fcPrint3 = new SFFunctionCall("print",
		SFLiteral_p(new SFString("A < B:")), SFLiteral_p(
			new SFFunctionCall("<", pGetA, pGetB)
		));
	SFFunctionCall *fcPrint4 = new SFFunctionCall("print",
		SFLiteral_p(new SFString("A > B:")), SFLiteral_p(
			new SFFunctionCall(">", pGetA, pGetB)
		));
	SFFunctionCall *fcPrint5 = new SFFunctionCall("print",
		SFLiteral_p(new SFString("A <= B:")), SFLiteral_p(
			new SFFunctionCall("<=", pGetA, pGetB)
		));
	SFFunctionCall *fcPrint6 = new SFFunctionCall("print",
		SFLiteral_p(new SFString("A >= B:")), SFLiteral_p(
			new SFFunctionCall(">=", pGetA, pGetB)
		));
	chain->push_back(SFLiteral_p(fcVarA));
	chain->push_back(SFLiteral_p(fcVarB));
	chain->push_back(SFLiteral_p(fcPrint1));
	chain->push_back(SFLiteral_p(fcPrint2));
	chain->push_back(SFLiteral_p(fcPrint3));
	chain->push_back(SFLiteral_p(fcPrint4));
	chain->push_back(SFLiteral_p(fcPrint5));
	chain->push_back(SFLiteral_p(fcPrint6));

	debug << chain->str() << std::endl;
	SFInterpreter si;
	SFLiteral_p result = si.run(chain_p);
	debug << "Result: " << result->str() << std::endl;
}

void basic_test();

void interp_test() {
	//basic_test();
	//test_comparison();
	test_factorial();
	test_fibonacci();
}

void basic_test() {
	// Test var, get
	// ((var A "foobar")
	//  (print "A:" (get 'A')))
	SFOpChain *ocTest = new SFOpChain();
	SFLiteral_p ocTest_p(ocTest);
	setupBuiltins();
	ocTest->importClosure(getBuiltinDefinitions());

	SFFunctionCall *fcVar = new SFFunctionCall("var", getAtomPtr("A"), SFLiteral_p(new SFString("foobar")));
	SFFunctionCall *fcGet = new SFFunctionCall("get", getAtomPtr("A"));
	SFFunctionCall *fcPrint = new SFFunctionCall("print", SFLiteral_p(new SFString("A:")), SFLiteral_p(fcGet));
	SFFunctionCall *fcPrint2 = new SFFunctionCall("print", SFLiteral_p(new SFString("woohoo!")));
	ocTest->push_back(SFLiteral_p(fcVar));
	ocTest->push_back(SFLiteral_p(fcPrint));
	ocTest->push_back(SFLiteral_p(fcPrint2));

	debug << ocTest->str() << std::endl;
	SFInterpreter si;
	SFLiteral_p result = si.run(ocTest_p);
	debug << "Result: " << result->str() << std::endl;
	ocTest->rewind();
	si.run(ocTest_p);
}

SFBasicList xtolist(const std::string &str) {
	SFBasicList l;
	std::string::const_iterator it = str.begin();
	for (; it != str.end(); ++it) {
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

void really_basic_tests() {
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
	c.set(pKey, pValue);
	debug << "Closure test: " << c.get(pKey)->str() << std::endl;
	// Ensure this only results in 1 item
	c.set(pKey, pValue2);
	assert(c.size() == 1);
	debug << "Closure test: " << c.get(pKey)->str() << std::endl;
#if defined(_DEBUG) && defined(_DEBUG_EXCEPTIONS)
	try {
		SFLiteral_p not_exist(c.get(str3));
		debug << "Closure test failure: " << not_exist << std::endl;
	} catch (std::runtime_error &e) {
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
	SFInterpreter *interp = new SFInterpreter();
	SFFnCallSignature_t params = { printParams, SFLiteral_p(new SFClosure(c)), interp};
	print(params);

	SFLiteral *integer1 = printParams[1].get();
	SFLiteral *integer2 = printParams[2].get();
	SFLiteral *integerAdded = *integer1 + *integer2;
	std::cout << "Integer added: " << integerAdded->str() << std::endl;
	delete integerAdded;

	SFLiteral *floatPi = new SFFloat(3.1415);
	SFLiteral *floatAdded = *floatPi + *integer1;
	std::cout << "Float added: " << floatAdded->str() << std::endl;
	delete floatPi;
	delete floatAdded;

	std::string fa1 = "foo";
	std::string fa2 = "foo/*";
	std::string fa3 = "foo/2";

	debug << "FA1: " << fa1 << " = " << getFunctionArity(fa1, 2).str() << std::endl;
	debug << "FA2: " << fa2 << " = " << getFunctionArity(fa2, 0).str() << std::endl;
	debug << "FA2: " << fa3 << " = " << getFunctionArity(fa3, 0).str() << std::endl;

	delete interp;
}
