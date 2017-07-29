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
		return args.interpreter.run(this->getBody());
	}

	void SFOpChain::importClosure(SFBuiltinDefinitions functions) {
		SFBuiltinDefinitions::iterator it = functions.begin();
		for (; it != functions.end(); it++) {
			SFNativeFunctionAttributes native = *it;
			SFAtom *atom = new SFAtom(getAtom(native.name));
			SFFunctionDefinitionNative *fndef = new SFFunctionDefinitionNative(native.name, native.args, native.body);
			SFLiteral_p fndef_p(fndef);
			SFLiteral_p atom_p(atom);
			this->getClosureObject()->setImmediate(atom_p, fndef_p);
		}
	}
}