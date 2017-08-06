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
		SFLiteral_p chain;
		SFInterpreter* interpreter;
	} SFFnCallSignature_t;
	typedef SFLiteral_p(*SFBuiltin_f)(const SFFnCallSignature_t&);
	typedef std::map<std::string, SFBuiltin_f> SFBuiltinMapString_t;
	typedef std::map<SFInteger_t, SFBuiltin_f> SFBuiltinMapAtom_t;

	typedef struct SFFunctionArity_s {
		std::string nameRawStr;
		std::string nameArityStr;
		std::string nameStarStr;
		SFLiteral_p nameArity;
		SFLiteral_p nameArityStar;
		char paramCount;
		bool isArityStar() const {
			return paramCount == '*';
		}
		std::string str() const {
			if (isArityStar())
				return nameStarStr;
			else
				return nameArityStr;
		}
	} SFFunctionArity_t;
	SFFunctionArity_t getFunctionArity(std::string name, const int np);

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
	} SFNativeFunctionAttributes_t;
	typedef std::vector<SFNativeFunctionAttributes_t> SFBuiltinDefinitions_t;
	typedef std::vector<SFInteger_t> SFFnDefArgsByAtom_t;

	typedef std::vector<std::string> DebugBuiltins_t;
	extern DebugBuiltins_t DebugBuiltins;

	typedef std::map<SFInteger_t, std::string> atomsById_t;
	typedef std::map<std::string, SFInteger_t> atomsByName_t;

	SFLiteral_p getAtomPtr(const std::string &name);
}