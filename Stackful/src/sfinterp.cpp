#include "../stdafx.h"

#include "../include/sfinterp.hpp"
#include "../include/sfextypes.hpp"
#include "../include/sfdebug.hpp"
#include "../include/sfbuiltins.hpp"
#include "../include/sffndef.hpp"

#include <exception>
#include <string>
#include <sstream>
#include <vector>

using namespace stackful;

namespace stackful {
	const SFInteger_t MaxDebugLen = 100;
	const SFInteger_t MaxDebugArrayLen = 20;

	DebugBuiltins_t DebugBuiltins({
		"while", "call", "try", "eval", "apply", "next", "recurse"
	});
	bool isDebugBuiltin(const std::string &name) {
		DebugBuiltins_t::iterator it = DebugBuiltins.begin();
		for (; it != DebugBuiltins.end(); ++it) {
			if (*it == name)
				return true;
		}
		return false;
	}

	SFLiteral_p SFInterpreter::run(SFLiteral_p chain_p) throw(std::runtime_error) {
		SFLiteral_p value(new SFAtom("nil"));
		SFOpChain *chain = toOpChain(chain_p);
		while (chain->next() != nullptr) {
			// Not const, as the instruction might be updated if it
			// references a function with incorrect arity.
			SFLiteral_p p = chain->get();
			if(false == p->isExtended())
				throw std::runtime_error("Invalid operation");
			SFExtended *i = static_cast<SFExtended*>(p.get());
			debug << "Instruction: " << i->str() << std::endl;
			switch (i->getExtendedType()) {
				case OpChain:
				{
					SFOpChain *op = static_cast<SFOpChain*>(i);
					SFOpChain *sub = new SFOpChain(chain_p, op);
					SFLiteral_p sub_p(sub);
					SFLiteral_p result = run(sub_p);
					value.swap(result);
					break;
				}
				case FunctionCall:
				{
					SFFunctionCall *fncall = static_cast<SFFunctionCall*>(i);
					SFLiteral_p result = doFunctionCall(chain_p, fncall);
					value.swap(result);
					if (value.get() != nullptr && value->isExtended()) {
						SFExtended *valueExt = value->ExtClass();
						if (valueExt->getExtendedType() == OpChain) {
							SFOpChain *op = toOpChain(value);
							if (op->getImmediate()) {
								SFOpChain *sub = new SFOpChain(chain_p, op);
								SFLiteral_p sub_p(sub);
								result = run(sub_p);
								value.swap(result);
							}
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

	/**
	* Perform a function call with the given chain closure.
	*
	* This is the main interpreting function, it takes care
	* of creating new closures for a function call, making
	* the call itself, and finding the correct function to
	* call based on arity.
	*
	* For each argument, if it is a function call, this
	* procedure is called recursively, eventually returning
	* the appropriate real value so that the function call
	* can occur.
	*/
	SFLiteral_p SFInterpreter::doFunctionCall(SFLiteral_p chain_p, SFFunctionCall *i) {
		SFFunctionArity_t details = i->getDetails();
		SFOpChain *chain = toOpChain(chain_p);
		SFLiteral_p fndef = chain->getClosureObject()->getOrMissing(i->getFunction());
		if (fndef == atomMissing) {
			fndef = chain->getClosureObject()->getOrMissing(details.nameArityStar);
			if (fndef == atomMissing) {
				throw std::runtime_error("Function not found: " + details.str());
			}
			debug << "Updated " + details.str() + " to arity * version" << std::endl;
			i->setFunction(details.nameArityStar);
		}
		SFList *args = i->getArguments();
		SFList::iterator it = args->begin();
		SFBasicList params;
		for (; it != args->end(); ++it) {
			params.push_back(this->getParamValue(chain_p, *it));
		}

		SFFnCallSignature_t call = { params, chain_p, this };
		return this->invokeFunctionCall(fndef, call);
	}
	SFLiteral_p SFInterpreter::getParamValue(SFLiteral_p chain_p, SFLiteral_p p) {
		SFLiteral *basic = p.get();
		if (basic->isExtended()) {
			SFExtended *ext = basic->ExtClass();
			if(ext->getExtendedType() == FunctionCall) {
				SFFunctionCall *fncall = static_cast<SFFunctionCall*>(ext);
				return this->doFunctionCall(chain_p, fncall);
			}
		}
		return p;
	}

	std::string SFInterpreter::inspectObject(const SFBasicList &obj) const {
		std::stringstream s;
		SFBasicList::iterator it = obj.begin();
		bool first = true;
		s << "[";
		for (; it != obj.end(); ++it) {
			if (first)
				first = false;
			else
				s << " ";
			SFLiteral *item = it->get();
			if (item->isExtended()) {
				SFExtended *ext = item->ExtClass();
				s << ext->extLiteral();
			} else {
				s << item->str();
			}
		}
		s << "]";
		return s.str();
	}

	std::string indentSymbol = "|";
	SFLiteral_p SFInterpreter::invokeFunctionCall(SFLiteral_p fndef, const SFFnCallSignature_t &call) {
		SFFunctionDefinitionBase *def = static_cast<SFFunctionDefinitionBase*>(fndef.get());
		SFFunctionArity_t details = def->getAttributes();
		std::stringstream s;
		SFLiteral_p result;

		++this->depth;

		if (debug.getEnabled()) {
			std::string indent;
			SFFunctionArity_t arity = def->getAttributes();
			if (this->depth < 20) {
				indent = std::string(indentSymbol, this->depth);
			} else {
				indent = "|             " + std::to_string(this->depth) + " | | ";
			}
			s << "+" << indent;
			s << " (" << arity.str();
			if(call.arguments.size() != 0)
				s << " " << this->inspectObject(call.arguments);
			s << ")";
		}
		if (def->isNative()) {
			if (debug.getEnabled() && isDebugBuiltin(details.nameRawStr)) {
				debug << s.str();
			}
		} else {
			// Mark opchain as function entry
			SFOpChain *chain = toOpChain(call.chain);
			chain->setFunctionEntry(details.nameArityStr);
		}
		result = def->invoke(call);
		depth--;
		if (debug.getEnabled()) {
			s << " :: ";
			SFBasicList inspector;
			inspector.push_back(result);
			s << this->inspectObject(inspector);
			debug << s.str() << std::endl;
		}
		return result;
	}
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

	void test_factorial();
	test_factorial();
}

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
	
	// (var N 10)
	//SFFunctionCall fcVarN = new SFFunctionCall("var", )
}
