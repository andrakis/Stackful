#pragma once

#include <map>
#include "sftypes.hpp"

namespace stackful {
	class SFInterpreter;
	class SFClosure;

	class SFOpChain;

	// Standard callable function signature
	typedef struct SFFnCallSignature_s {
		SFBasicList arguments;
		SFLiteral_p closure;
		SFInterpreter& interpreter;
	} SFFnCallSignature_t;
	typedef SFLiteral_p(*SFBuiltin_f)(SFFnCallSignature_t);
	typedef std::map<std::string, SFBuiltin_f> SFBuiltinMapString_t;
	typedef std::map<SFInteger_t, SFBuiltin_f> SFBuiltinMapAtom_t;

	typedef std::tuple<std::string, char> SFFunctionArity_t;

	// Builtin function param list
	typedef std::vector<std::string> SFBuiltinParams_t;

	typedef std::map<int, SFLiteral_p> SFClosureKeyValue;
	typedef std::map<SFInteger_t,SFClosureKeyValue> SFClosureFunctionsByAtom;

	typedef std::string SFFnDefName;
	typedef std::vector<std::string> SFFnDefArgs;
	typedef struct SFNativeFunctionAttributes_s {
		SFFnDefName name;
		SFFnDefArgs args;
		SFBuiltin_f body;
	} SFNativeFunctionAttributes;
	typedef std::vector<SFNativeFunctionAttributes> SFBuiltinDefinitions;

	typedef std::vector<std::string> DebugBuiltins_t;
	extern DebugBuiltins_t DebugBuiltins;
}