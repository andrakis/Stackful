#include "../include/sffndef.hpp"

namespace stackful {
	SFFunctionDefinitionNative::SFFunctionDefinitionNative(const SFFnDefName &_name, const SFFnDefArgs &_args, const SFBuiltin_f &_body)
	: SFFunctionDefinitionBase(FunctionDefinitionNative, _name, _args), body(_body) {
	}

	SFLiteral_p SFFunctionDefinitionNative::invoke(const SFFnCallSignature_t &_args) {
		return body(_args);
	}

	SFFunctionDefinition::SFFunctionDefinition(SFLiteral_p parent, const SFFnDefName &_name, const SFFnDefArgs &_args, SFLiteral_p body)
	: SFFunctionDefinitionBase(FunctionDefinition, _name, _args) {
		this->push_back(body);
	}

	SFLiteral_p SFFunctionDefinition::invoke(const SFFnCallSignature_t &_args) {
		SFLiteral_p body_p = this->getBody();
		SFOpChain *body = toOpChain(body_p);
		SFLiteral_p parent_p = _args.chain;
		if (this->isScoped())
			parent_p = this->getScope();
		SFOpChain *chain = new SFOpChain(parent_p, body);
		SFLiteral_p chain_p(chain);
		// Set arguments in new chain
		SFFnDefArgsByAtom_t ourArgs = this->getArgsByAtom();
		SFFnDefArgsByAtom_t::iterator it = ourArgs.begin();
		int position = 0;
		SFFunctionArity_t fnattributes = this->getAttributes();
		SFBasicList arguments = _args.arguments;
		if (fnattributes.isArityStar()) {
			// Converts args to [args]
			SFBasicList tmpList;
			tmpList.push_back(arguments);
			arguments = tmpList;
		}
		// Set variables in new scope
		for (; it != ourArgs.end(); ++it, ++position) {
			// Get atom by id
			SFInteger_t atomId = *it;
			SFLiteral_p _name = getAtomPtr(atomId);
			chain->getClosureObject()->setImmediate(_name, arguments[position]);
		}
		return _args.interpreter->run(chain_p);
	}

	void SFOpChain::importClosure(const SFBuiltinDefinitions_t &functions) {
		SFBuiltinDefinitions_t::const_iterator it = functions.begin();
		for (; it != functions.end(); ++it) {
			SFNativeFunctionAttributes_t native = *it;
			SFFunctionDefinitionNative *fndef = new SFFunctionDefinitionNative(native.name, native.args, native.body);
			const SFFunctionArity_t &attrs = fndef->getAttributes();
			SFLiteral_p fndef_p(fndef);
			SFLiteral_p atom_p = attrs.nameArity;
			this->getClosureObject()->setImmediate(atom_p, fndef_p);
		}
	}
}
