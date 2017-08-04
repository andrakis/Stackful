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

void stackful::setupBuiltins() {
	if (definitionsDone)
		return;
	definitionsDone = true;
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
