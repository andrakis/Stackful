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
	addBuiltin("print/*", params(), [](SFList &parameters, SFClosure_p closure) {
		std::stringstream s;
		bool first = true;
		for (size_t i = 0; i < parameters.size(); i++) {
			if (first)
				first = false;
			else
				s << " ";
			s << varstr(parameters[i].get()->ListClass());
		}
		std::cout << s.str() << std::endl;
		return SFLiteral_p(new SFList(sfvar(atomNil)));
	});

	addBuiltin("var", params("Name", "Value"), [](SFList &parameters, SFClosure_p closure) {
		closure.get()->setImmediate(parameters[0].get()->ListClass(), parameters[1]);
		return parameters[1];
	});

	addBuiltin("set", params("Name", "Value"), [](SFList &parameters, SFClosure_p closure) {
		closure.get()->set(parameters[0].get()->ListClass(), parameters[1]);
		return parameters[1];
	});

	addBuiltin("get", params("Name"), [](SFList &parameters, SFClosure_p closure) {
		return closure.get()->get(parameters[0].get()->ListClass());
	});
}

SFBuiltin_f getBuiltin(const SFInteger_t atomId) {
	return builtinsByAtom[atomId];
}

SFBuiltin_f getBuiltin(const std::string &name) {
	return builtinsByString[name];
}