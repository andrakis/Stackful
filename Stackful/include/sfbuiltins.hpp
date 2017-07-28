#pragma once

#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "sfextypes.hpp"

namespace stackful {
	class SFInterpreter;
	// Standard builtin function signature
	typedef struct SFBuiltinSignature_s {
		SFBasicList& arguments;
		SFClosure_p closure;
		SFInterpreter& interpreter;
	} SFBuiltinSignature_t;
	typedef SFLiteral_p(*SFBuiltin_f)(SFBuiltinSignature_t);
	typedef std::map<std::string, SFBuiltin_f> SFBuiltinMapString_t;
	typedef std::map<SFInteger_t, SFBuiltin_f> SFBuiltinMapAtom_t;

	typedef std::tuple<std::string, char> SFFunctionArity_t;

	// Builtin function param list
	typedef std::vector<std::string> SFBuiltinParams_t;

	SFBuiltinParams_t params();
	SFBuiltinParams_t params(std::string);
	SFBuiltinParams_t params(std::string, std::string);
	SFBuiltinParams_t params(std::string, std::string, std::string);
	SFBuiltinParams_t params(std::string, std::string, std::string, std::string);

	void setupBuiltins();
	SFBuiltin_f getBuiltin(const SFInteger_t atomId);
	SFBuiltin_f getBuiltin(const std::string &name);

	SFFunctionArity_t getFunctionArity(const std::string &name, const int np);
	std::string fastr(const SFFunctionArity_t fa);

}
