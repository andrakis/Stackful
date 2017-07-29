#pragma once

#include <map>
#include "sftypes.hpp"

namespace stackful {
	class SFInterpreter;
	class SFClosure;

	class SFOpChain;

	typedef std::shared_ptr<SFOpChain> SFOpChain_p;
	typedef std::shared_ptr<SFClosure> SFClosure_p;

	// Standard builtin function signature
	typedef struct SFBuiltinSignature_s {
		SFBasicList arguments;
		SFClosure_p closure;
		SFInterpreter& interpreter;
	} SFBuiltinSignature_t;
	typedef SFLiteral_p(*SFBuiltin_f)(SFBuiltinSignature_t);
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

}