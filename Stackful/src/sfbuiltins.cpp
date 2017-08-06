//
// Stackful builtin functions
//

#include "../stdafx.h"
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "../include/sfatoms.hpp"
#include "../include/sfbuiltins.hpp"
#include "../include/sfextypes.hpp"
#include "../include/sffndef.hpp"

using namespace stackful;

SFInteger_t builtinCounter = 0;

SFBuiltinMapAtom_t builtinsByAtom;
SFBuiltinMapString_t builtinsByString;
SFBuiltinDefinitions_t builtinDefinitions;

SFBuiltinParams_t stackful::params() {
	return SFBuiltinParams_t();
}
SFBuiltinParams_t stackful::params(const std::string &p1) {
	SFBuiltinParams_t p;
	p.push_back(p1);
	return p;
}
SFBuiltinParams_t stackful::params(const std::string &p1, const std::string &p2) {
	SFBuiltinParams_t p;
	p.push_back(p1);
	p.push_back(p2);
	return p;
}
SFBuiltinParams_t stackful::params(const std::string &p1, const std::string &p2, const std::string &p3) {
	SFBuiltinParams_t p;
	p.push_back(p1);
	p.push_back(p2);
	p.push_back(p3);
	return p;
}
stackful::SFBuiltinParams_t params(const std::string &p1, const std::string &p2, const std::string &p3, const std::string &p4) {
	SFBuiltinParams_t p;
	p.push_back(p1);
	p.push_back(p2);
	p.push_back(p3);
	p.push_back(p4);
	return p;
}

void addBuiltin(std::string name, SFBuiltinParams_t parameters, SFBuiltin_f fn) {
	SFNativeFunctionAttributes_t attrs = { name, parameters, fn };
	builtinDefinitions.push_back(attrs);

	SFInteger_t atomId = getAtom(name);
	builtinsByAtom.emplace(atomId, fn);
	builtinsByString.emplace(name, fn);
}

volatile bool definitionsDone = false;
SFClosure *getClosure(SFLiteral_p chain) {
	return toOpChain(chain)->getClosureObject();
}
SFClosure *getClosure(SFOpChain *chain) {
	return chain->getClosureObject();
}

SFLiteral_p getIfResult(SFLiteral_p value_p, SFLiteral_p chain_p) {
	const SFLiteral* value = value_p.get();
	if (value->isExtended()) {
		const SFExtended* ext = value->ExtClass();
		if (ext->getExtendedType() == OpChain) {
			const SFOpChain* chain = static_cast<const SFOpChain*>(ext);
			SFOpChain* immediate = new SFOpChain(chain_p, chain);
			immediate->setImmediate(true);
			return SFLiteral_p(immediate);
		}
	}
	return value_p;
}

SFLiteral_p emptyChain;

SFLiteral_p TestIf(SFLiteral_p Test, SFLiteral_p Action, SFLiteral_p Else, SFLiteral_p chain) {
	if (Test == atomTrue)
		return getIfResult(Action, chain);
	else
		return getIfResult(Else, chain);
}

void stackful::setupBuiltins() {
	if (definitionsDone)
		return;
	definitionsDone = true;

	emptyChain = SFLiteral_p(new SFOpChain());

	addBuiltin("print/*", params(), [](SFFnCallSignature_t params) {
		std::stringstream s;
		bool first = true;
		for (size_t i = 0; i < params.arguments.size(); i++) {
			if (first)
				first = false;
			else
				s << " ";
			SFLiteral_p p = params.arguments[i];
			s << p->str();
		}
		std::cout << s.str() << std::endl;
		return atomNil;
	});

	addBuiltin("var", params("Name", "Value"), [](SFFnCallSignature_t params) {
		getClosure(params.chain)->setImmediate(params.arguments[0], params.arguments[1]);
		return params.arguments[1];
	});

	addBuiltin("set", params("Name", "Value"), [](SFFnCallSignature_t params) {
		getClosure(params.chain)->set(params.arguments[0], params.arguments[1]);
		return params.arguments[1];
	});

	addBuiltin("get", params("Name"), [](SFFnCallSignature_t params) {
		return getClosure(params.chain)->get(params.arguments[0]);
	});

	addBuiltin("==", params("A", "B"), [](SFFnCallSignature_t params) {
		return *params.arguments[0] == *params.arguments[1] ? atomTrue : atomFalse;
	});
	addBuiltin("!=", params("A", "B"), [](SFFnCallSignature_t params) {
		return *params.arguments[0] != *params.arguments[1] ? atomTrue : atomFalse;
	});
	addBuiltin("<", params("A", "B"), [](SFFnCallSignature_t params) {
		return *params.arguments[0] < *params.arguments[1] ? atomTrue : atomFalse;
	});
	addBuiltin(">", params("A", "B"), [](SFFnCallSignature_t params) {
		return *params.arguments[0] > *params.arguments[1] ? atomTrue : atomFalse;
	});
	addBuiltin("<=", params("A", "B"), [](SFFnCallSignature_t params) {
		return *params.arguments[0] <= *params.arguments[1] ? atomTrue : atomFalse;
	});
	addBuiltin(">=", params("A", "B"), [](SFFnCallSignature_t params) {
		return *params.arguments[0] >= *params.arguments[1] ? atomTrue : atomFalse;
	});

	addBuiltin("def", params("Name", "Function"), [](SFFnCallSignature_t params) {
		const SFFunctionDefinitionNative *native = static_cast<SFFunctionDefinitionNative*>(params.arguments[1].get());
		SFFunctionArity_t attrs = getFunctionArity(params.arguments[0]->str(), native->getArgs().size());
		toOpChain(params.chain)->getClosureObject()->setImmediate(attrs.nameArity, params.arguments[1]);
		return params.arguments[1];
	});

	addBuiltin("+", params("A", "B"), [](SFFnCallSignature_t params) {
		SFLiteral_p result = params.arguments.foldLeft([](SFLiteral_p acc, SFLiteral_p curr) {
			return SFLiteral_p(*acc + *curr);
		});
		return result;
	});
	addBuiltin("-", params("A", "B"), [](SFFnCallSignature_t params) {
		SFLiteral_p result = params.arguments.foldLeft([](SFLiteral_p acc, SFLiteral_p curr) {
			return SFLiteral_p(*acc - *curr);
		});
		return result;
	});
	addBuiltin("*", params("A", "B"), [](SFFnCallSignature_t params) {
		SFLiteral_p result = params.arguments.foldLeft([](SFLiteral_p acc, SFLiteral_p curr) {
			return SFLiteral_p(*acc * *curr);
		});
		return result;
	});
	addBuiltin("/2", params("A", "B"), [](SFFnCallSignature_t params) {
		SFLiteral_p result = params.arguments.foldLeft([](SFLiteral_p acc, SFLiteral_p curr) {
			return SFLiteral_p(*acc / *curr);
		});
		return result;
	});
	addBuiltin("<<", params("A", "B"), [](SFFnCallSignature_t params) {
		SFLiteral_p result = params.arguments.foldLeft([](SFLiteral_p acc, SFLiteral_p curr) {
			return SFLiteral_p(*acc << *curr);
		});
		return result;
	});
	addBuiltin(">>", params("A", "B"), [](SFFnCallSignature_t params) {
		SFLiteral_p result = params.arguments.foldLeft([](SFLiteral_p acc, SFLiteral_p curr) {
			return SFLiteral_p(*acc >> *curr);
		});
		return result;
	});

	addBuiltin("if/2", params("Test", "Action"), [](SFFnCallSignature_t params) {
		return TestIf(params.arguments[0], params.arguments[1], emptyChain, params.chain);
	});
	addBuiltin("if/3", params("Test", "Action", "Else"), [](SFFnCallSignature_t params) {
		return TestIf(params.arguments[0], params.arguments[1], params.arguments[2], params.chain);
	});

	addBuiltin("list/*", params(), [](SFFnCallSignature_t params) {
		SFList *list = new SFList();
		list->ShallowCopy(params.arguments);
		return SFLiteral_p(list);
	});
}

SFBuiltin_f stackful::getBuiltin(const SFInteger_t atomId) {
	return builtinsByAtom[atomId];
}

SFBuiltin_f stackful::getBuiltin(const std::string &name) {
	return builtinsByString[name];
}

SFBuiltinDefinitions_t stackful::getBuiltinDefinitions() {
	return builtinDefinitions;
}
