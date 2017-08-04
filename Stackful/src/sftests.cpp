#include "../include/sfinterp.hpp"

using namespace stackful;

void test_factorial() {
	// Test a factorial function
	// ((def fac #N :: (
	//    (if (<= N 0) (
	//       (1)
	//    ) (else (
	//       (* N (- N 1))
	//    )))
	//  ))
	//  (var N 10)
	//  (print "Fac of" (get N) (fac (get N)))
	// )
	SFOpChain *chain = new SFOpChain();
	SFLiteral_p chain_p(chain);

	chain->importClosure(getBuiltinDefinitions());

	debug << chain->str() << std::endl;
	SFInterpreter si;
	SFLiteral_p result = si.run(chain_p);
	debug << "Result: " << result->str() << std::endl;
	
	// (var N 10)
	//SFFunctionCall fcVarN = new SFFunctionCall("var", )
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

void interp_test () {
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

	test_comparison();
	test_factorial();
}

