#include "../include/sffndef.hpp"

namespace stackful {
	SFFunctionDefinitionNative::SFFunctionDefinitionNative(SFFnDefName _name, SFFnDefArgs _args, SFBuiltin_f _body)
	: SFFunctionDefinitionBase(FunctionDefinitionNative, _name, _args), body(_body) {
	}

	SFLiteral_p SFFunctionDefinitionNative::invoke(SFFnCallSignature_t args) {
		return body(args);
	}

	SFFunctionDefinition::SFFunctionDefinition(SFLiteral_p parent, SFFnDefName _name, SFFnDefArgs _args, SFLiteral_p body)
	: SFFunctionDefinitionBase(FunctionDefinition, _name, _args) {
		this->push_back(body);
	}

	SFLiteral_p SFFunctionDefinition::invoke(SFFnCallSignature_t args) {
		SFLiteral_p body_p = this->getBody();
		SFLiteral_p parent_p = args.closure;
		if (this->isScoped())
			parent_p = this->getScope();
		SFOpChain *body = toOpChain(body_p);
		SFOpChain *chain = new SFOpChain(parent_p, body);
		SFLiteral_p chain_p(chain);
		// Set arguments in new chain
		SFFnDefArgsByAtom_t ourArgs = this->getArgsByAtom();
		SFFnDefArgsByAtom_t::iterator it = ourArgs.begin();
		int position = 0;
		SFFunctionArity_t fnattributes = this->getAttributes();
		SFBasicList arguments = args.arguments;
		if (fnattributes.isArityStar()) {
			SFBasicList tmpList;
			tmpList.push_back(arguments);
			arguments = tmpList;
		}
		for (; it != ourArgs.end(); it++, ++position) {
			// Get atom by id
			SFInteger_t atomId = *it;
			SFLiteral_p name = getAtomPtr(atomId);
			chain->getClosureObject()->setImmediate(name, arguments[position]);
		}
		return args.interpreter.run(chain_p);
	}

	void SFOpChain::importClosure(SFBuiltinDefinitions functions) {
		SFBuiltinDefinitions::iterator it = functions.begin();
		for (; it != functions.end(); it++) {
			SFNativeFunctionAttributes native = *it;
			SFFunctionDefinitionNative *fndef = new SFFunctionDefinitionNative(native.name, native.args, native.body);
			SFLiteral_p fndef_p(fndef);
			SFLiteral_p atom_p = getAtomPtr(native.name);
			this->getClosureObject()->setImmediate(atom_p, fndef_p);
		}
	}
}