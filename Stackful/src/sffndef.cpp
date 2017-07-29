#include "../include/sffndef.hpp"

namespace stackful {
	SFFunctionDefinitionNative::SFFunctionDefinitionNative(SFFnDefName _name, SFFnDefArgs _args, SFBuiltin_f _body)
	: SFFunctionDefinitionBase(FunctionDefinitionNative, _name, _args), body(_body) {
	}

	SFFunctionDefinition::SFFunctionDefinition(SFOpChain_p parent, SFFnDefName _name, SFFnDefArgs _args, SFOpChain_p body)
	: SFFunctionDefinitionBase(FunctionDefinition, _name, _args) {
	}

	void SFOpChain::importClosure(SFBuiltinDefinitions functions) {
		SFBuiltinDefinitions::iterator it = functions.begin();
		for (; it != functions.end(); it++) {
			SFNativeFunctionAttributes native = *it;
			SFAtom *atom = new SFAtom(getAtom(native.name));
			SFFunctionDefinitionNative *fndef = new SFFunctionDefinitionNative(native.name, native.args, native.body);
			SFLiteral_p fndef_p(fndef);
			SFLiteral_p atom_p(atom);
			this->closure->setImmediate(atom_p, fndef_p);
		}
	}
}