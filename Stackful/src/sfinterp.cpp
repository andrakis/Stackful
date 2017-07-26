#include "../include/sfextypes.hpp"

#include <exception>

using namespace stackful;

class SFInterpreter {
public:
	SFInterpreter () { }
	~SFInterpreter () { }
	SFLiteral_p run(SFOpChain &chain) const throw(std::runtime_error) {
		SFLiteral_p value(new SFAtom("nil"));
		while(chain.next() != nullptr) {
			// Not const, as the instruction might be updated if it
			// references a function with incorrect arity.
			SFExtended *i = chain.get();
			debug << "Instruction: " << i->str() << std::endl;
			switch(i->getExtendedType()) {
				case FunctionCall:
				{
					SFLiteral_p fncall = do_functioncall(chain, i);
					value.swap(fncall);
					break;
				}
				default:
					throw(std::runtime_error("Unknown type"));
			}
		}
		return value;
	}
protected:
	SFLiteral_p do_functioncall(SFOpChain &chain, SFExtended *i) const {
		return SFLiteral_p(new SFAtom("todo"));
	}
};

void interp_test () {
	// Test var, get
	// ((var A "foobar")
	//  (print "A:" (get 'A')))
	SFOpChain ocTest;
	SFFunctionCall *fcVar = new SFFunctionCall("var", SFLiteral_p(new SFAtom("A")), SFLiteral_p(new SFString("foobar")));
	SFFunctionCall *fcGet = new SFFunctionCall("get", SFLiteral_p(new SFAtom("A")));
	SFFunctionCall *fcPrint = new SFFunctionCall("print", SFLiteral_p(new SFString("A:")), SFLiteral_p(fcGet));
	ocTest.push_back(SFLiteral_p(fcVar));
	ocTest.push_back(SFLiteral_p(fcPrint));

	debug << ocTest.str() << std::endl;
	SFInterpreter si;
	SFLiteral_p result = si.run(ocTest);
	debug << "Result: " << result->str() << std::endl;
}
