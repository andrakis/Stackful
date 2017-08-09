/*
 * Stackful Interpreter
 * Runs OpChains by satisfying function calls.
 **/
#include "../stdafx.h"

#include "../include/sfinterp.hpp"
#include "../include/sfextypes.hpp"
#include "../include/sfdebug.hpp"
#include "../include/sfbuiltins.hpp"
#include "../include/sffndef.hpp"

#include <exception>
#include <string>
#include <sstream>

namespace stackful {
	const SFInteger_t MaxDebugLen = 100;
	const SFInteger_t MaxDebugArrayLen = 20;

#ifdef _STATS
#define STATS_INC(which)  do { ++this->stats.which; } while(0)
#else
#define STATS_INC(which)
#endif

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

	SFLiteral_p SFInterpreter::getFnDefFromClosure(SFClosure *closure, SFFunctionCall *i, const SFFunctionArity_t &details) const
	{
		SFLiteral_p fndef = closure->getOrMissing(i->getFunction());
		if (fndef == atomMissing) {
			fndef = closure->getOrMissing(details.nameArityStar);
			if (fndef == atomMissing)
				return atomMissing;
			i->setFunction(details.nameArityStar);
		}
		return fndef;

	}

	SFLiteral_p SFInterpreter::getFnDef(const SFOpChain *chain, SFFunctionCall *i, const SFFunctionArity_t &details) throw(std::runtime_error) {
		SFClosure *current = chain->getClosureObject();
		SFClosure *topmost = current->getTopmostObject();
		SFLiteral_p fndef = this->getFnDefFromClosure(topmost, i, details);
		if (fndef == atomMissing) {
			STATS_INC(topmost_misses);
			fndef = this->getFnDefFromClosure(current, i, details);
			if (fndef == atomMissing) {
				throw std::runtime_error("Function not found: " + details.str());
			}
		} else {
			STATS_INC(topmost_hits);
		}
		return fndef;
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
		SFLiteral_p fndef = this->getFnDef(chain, i, details);
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
		STATS_INC(resolutions);
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
		return this->inspectObject(obj, "[", "]");
	}
	std::string SFInterpreter::inspectObject(const SFBasicList &obj, const std::string &pre, const std::string &post) const {
		std::stringstream s;
		SFBasicList::iterator it = obj.begin();
		bool first = true;
		s << pre;
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
		s << post;
		return s.str();
	}

	char indentSymbol = '|';
	SFLiteral_p SFInterpreter::invokeFunctionCall(const SFLiteral_p &fndef, const SFFnCallSignature_t &call) {
		SFFunctionDefinitionBase *def = static_cast<SFFunctionDefinitionBase*>(fndef.get());
		SFFunctionArity_t details = def->getAttributes();
		std::stringstream s;
		SFLiteral_p result;

		++this->depth;
		STATS_INC(function_calls);

		if (debug.getEnabled()) {
			std::string indent;
			SFFunctionArity_t arity = def->getAttributes();
			if (this->depth < 20) {
				indent = std::string(this->depth, indentSymbol);
			} else {
				indent = "|             " + std::to_string(this->depth) + " | | ";
			}
			s << "+ " << indent;
			s << " (" << arity.str();
			if(call.arguments.size() != 0)
				s << " " << this->inspectObject(call.arguments);
			s << ")";
		}
		if (def->isNative()) {
			if (debug.getEnabled() && isDebugBuiltin(details.nameRawStr)) {
				debug << s.str() << std::endl;
			}
		} else {
			// Mark opchain as function entry
			SFOpChain *chain = toOpChain(call.chain);
			chain->setFunctionEntry(details.nameArityStr);
			debug << s.str() << std::endl;
		}
		result = def->invoke(call);
		depth--;
		if (debug.getEnabled()) {
			s << " :: ";
			SFBasicList inspector;
			inspector.push_back(result);
			s << this->inspectObject(inspector, "", "");
			debug << s.str() << std::endl;
		}
		return result;
	}
}
