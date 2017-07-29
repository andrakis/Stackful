//
// Stackful builtin functions
//

#include <iostream>
#include <map>
#include <regex>
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

std::regex functionArityRegex(R"~(([a-z][a-z0-9_]+)\/?(\*|\d+)$)~");
SFFunctionArity_t stackful::getFunctionArity(const std::string &name, const int np) {
	std::sregex_iterator iter(name.begin(), name.end(), functionArityRegex);
	std::sregex_iterator end;
	if (iter == end) {
		return SFFunctionArity_t(name, np);
	}

	auto fnName = (*iter)[1];
	auto fnArity = (*iter)[2];
	if(fnArity == "*")
		return SFFunctionArity_t(fnName, '*');
	return SFFunctionArity_t(fnName, std::stoi(fnArity));
}

std::string stackful::fastr(const SFFunctionArity_t fa) {
	std::stringstream s;
	s << std::get<0>(fa);
	s << "/";
	char arity = std::get<1>(fa);
	if (arity == '*')
		s << "*";
	else
		s << (int)arity;
	return s.str();
}


SFBuiltinParams_t stackful::params() {
	return SFBuiltinParams_t();
}
SFBuiltinParams_t stackful::params(std::string p1) {
	SFBuiltinParams_t p;
	p.push_back(p1);
	return p;
}
SFBuiltinParams_t stackful::params(std::string p1, std::string p2) {
	SFBuiltinParams_t p;
	p.push_back(p1);
	p.push_back(p2);
	return p;
}
SFBuiltinParams_t stackful::params(std::string p1, std::string p2, std::string p3) {
	SFBuiltinParams_t p;
	p.push_back(p1);
	p.push_back(p2);
	p.push_back(p3);
	return p;
}
stackful::SFBuiltinParams_t params(std::string p1, std::string p2, std::string p3, std::string p4) {
	SFBuiltinParams_t p;
	p.push_back(p1);
	p.push_back(p2);
	p.push_back(p3);
	p.push_back(p4);
	return p;
}

void addBuiltin(std::string name, SFBuiltinParams_t parameters, SFBuiltin_f fn) {
	SFInteger_t builtinId = builtinCounter++;
	SFInteger_t atomId = getAtom(name);
	builtinsByAtom.emplace(atomId, fn);
	builtinsByString.emplace(name, fn);
}

void stackful::setupBuiltins() {
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
		return SFLiteral_p(new SFBasicList(sfvar(atomNil)));
	});

	addBuiltin("var", params("Name", "Value"), [](SFFnCallSignature_t params) {
		toClosure(params.closure)->setImmediate(params.arguments[0], params.arguments[1]);
		return params.arguments[1];
	});

	addBuiltin("set", params("Name", "Value"), [](SFFnCallSignature_t params) {
		toClosure(params.closure)->set(params.arguments[0], params.arguments[1]);
		return params.arguments[1];
	});

	addBuiltin("get", params("Name"), [](SFFnCallSignature_t params) {
		return toClosure(params.closure)->get(params.arguments[0]);
	});
}

SFBuiltin_f stackful::getBuiltin(const SFInteger_t atomId) {
	return builtinsByAtom[atomId];
}

SFBuiltin_f stackful::getBuiltin(const std::string &name) {
	return builtinsByString[name];
}
