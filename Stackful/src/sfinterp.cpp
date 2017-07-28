#include "../include/sfextypes.hpp"

#include <exception>
#include <string>
#include <vector>

using namespace stackful;

namespace stackful {
	const SFInteger_t MaxDebugLen = 100;
	const SFInteger_t MaxDebugArrayLen = 20;

	typedef std::vector<std::string> DebugBuiltins_t;
	DebugBuiltins_t DebugBuiltins({
		"while", "call", "try", "eval", "apply", "next", "recurse"
	});

	class SFInterpreter {
	public:
		SFInterpreter() : debugMode(true) { }
		~SFInterpreter() { }
		SFLiteral_p run(SFOpChain_p &chain_p) throw(std::runtime_error) {
			SFLiteral_p value(new SFAtom("nil"));
			SFOpChain &chain = *chain_p;
			while (chain.next() != nullptr) {
				// Not const, as the instruction might be updated if it
				// references a function with incorrect arity.
				SFLiteral_p p = chain.get();
				if(false == p->isExtended())
					throw std::runtime_error("Invalid operation");
				SFExtended *i = static_cast<SFExtended*>(p.get());
				debug << "Instruction: " << i->str() << std::endl;
				switch (i->getExtendedType()) {
					case OpChain:
					{
						SFOpChain *op = static_cast<SFOpChain*>(i);
						SFOpChain *sub = new SFOpChain(chain_p, op);
						SFOpChain_p sub_p(sub);
						SFLiteral_p result = run(sub_p);
						value.swap(result);
						break;
					}
					case FunctionCall:
					{
						SFFunctionCall *fncall = static_cast<SFFunctionCall*>(i);
						SFLiteral_p result = do_functioncall(chain, fncall);
						value.swap(result);
						if (value.get() != nullptr && value->isExtended()) {
							SFExtended *valueExt = value->ExtClass();
							if (valueExt->getExtendedType() == OpChain) {
								SFOpChain *op = static_cast<SFOpChain*>(value.get());
								SFOpChain *sub = new SFOpChain(chain_p, op);
								SFOpChain_p sub_p(sub);
								SFLiteral_p result = run(sub_p);
								value.swap(result);
							}
						}
						break;
					}
					default:
						value.swap(p);
						break;
				}
			}
			return value;
		}
		SFInteger_t getDepth() const { return depth; }
		bool getDebug() { return debugMode; }
	protected:
		SFInteger_t depth = 0;
		SFLiteral_p do_functioncall(SFOpChain &chain, SFExtended *i) const {
			return SFLiteral_p(new SFAtom("todo"));
		}
		bool debugMode;
	};
}

void interp_test () {
	// Test var, get
	// ((var A "foobar")
	//  (print "A:" (get 'A')))
	SFOpChain *ocTest = new SFOpChain();
	SFFunctionCall *fcVar = new SFFunctionCall("var", SFLiteral_p(new SFAtom("A")), SFLiteral_p(new SFString("foobar")));
	SFFunctionCall *fcGet = new SFFunctionCall("get", SFLiteral_p(new SFAtom("A")));
	SFFunctionCall *fcPrint = new SFFunctionCall("print", SFLiteral_p(new SFString("A:")), SFLiteral_p(fcGet));
	ocTest->push_back(SFLiteral_p(fcVar));
	ocTest->push_back(SFLiteral_p(fcPrint));

	SFOpChain_p ocTest_p(ocTest);

	debug << ocTest->str() << std::endl;
	SFInterpreter si;
	SFLiteral_p result = si.run(ocTest_p);
	debug << "Result: " << result->str() << std::endl;
}
