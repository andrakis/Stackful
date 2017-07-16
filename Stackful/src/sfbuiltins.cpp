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

SFInteger_t builtinCounter = 0;

SFBuiltinMapAtom_t builtinsByAtom;
SFBuiltinMapString_t builtinsByString;

std::regex functionArityRegex(R"~(([a-z][a-z0-9_]+)\/?(\*|\d+)$)~");
SFFunctionArity_t getFunctionArity(const std::string &name, const int np) {
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

std::string fastr(const SFFunctionArity_t fa) {
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


SFBuiltinParams_t params() {
	return SFBuiltinParams_t();
}
SFBuiltinParams_t params(std::string p1) {
	SFBuiltinParams_t p;
	p.push_back(p1);
	return p;
}
SFBuiltinParams_t params(std::string p1, std::string p2) {
	SFBuiltinParams_t p;
	p.push_back(p1);
	p.push_back(p2);
	return p;
}
SFBuiltinParams_t params(std::string p1, std::string p2, std::string p3) {
	SFBuiltinParams_t p;
	p.push_back(p1);
	p.push_back(p2);
	p.push_back(p3);
	return p;
}
SFBuiltinParams_t params(std::string p1, std::string p2, std::string p3, std::string p4) {
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

void setupBuiltins() {
	addBuiltin("print/*", params(), [](SFBasicList &parameters, SFClosure_p closure) {
		std::stringstream s;
		bool first = true;
		for (size_t i = 0; i < parameters.size(); i++) {
			if (first)
				first = false;
			else
				s << " ";
			SFLiteral_p p = parameters[i];
			s << p->str();
		}
		std::cout << s.str() << std::endl;
		return SFLiteral_p(new SFBasicList(sfvar(atomNil)));
	});

	addBuiltin("var", params("Name", "Value"), [](SFBasicList &parameters, SFClosure_p closure) {
		closure->setImmediate(parameters[0]->ListClass(), parameters[1]);
		return parameters[1];
	});

	addBuiltin("set", params("Name", "Value"), [](SFBasicList &parameters, SFClosure_p closure) {
		closure->set(parameters[0]->ListClass(), parameters[1]);
		return parameters[1];
	});

	addBuiltin("get", params("Name"), [](SFBasicList &parameters, SFClosure_p closure) {
		return closure->get(parameters[0]->ListClass());
	});
}

SFBuiltin_f getBuiltin(const SFInteger_t atomId) {
	return builtinsByAtom[atomId];
}

SFBuiltin_f getBuiltin(const std::string &name) {
	return builtinsByString[name];
}
