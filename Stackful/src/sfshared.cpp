#include "../stdafx.h"
#include <exception>
#include <regex>
#include "../include/sfatoms.hpp"
#include "../include/sftypes.hpp"
#include "../include/sfextypes.hpp"

namespace stackful {
	SFLiteral_p atomNil(new SFAtom(atomNilId)),
		atomFalse(new SFAtom(atomFalseId)),
		atomTrue(new SFAtom(atomTrueId)),
		atomMissing(new SFAtom(atomMissingId));

	std::regex functionArityRegex(R"~(([a-z][a-z0-9_]+)\/?(\*|\d+)?$)~");
	SFFunctionArity_t getFunctionArity(std::string name, const int np) {
		std::sregex_iterator iter(name.begin(), name.end(), functionArityRegex);
		std::sregex_iterator end;
		if (iter == end) {
			throw std::runtime_error("Invalid function signature");
		}

		auto fnName = (*iter)[1];
		auto fnArity = (*iter)[2];
		std::string fnNameRaw = fnName.str();
		std::string fnNameStr = fnNameRaw + "/" + std::to_string(np);
		std::string fnArityStr = fnNameRaw + "/*";
		if (fnArity == "*")
			return SFFunctionArity_t({ fnNameRaw, name, name, getAtomPtr(name), getAtomPtr(name), '*' });
		char nparams = (int)np;
		if (fnArity.matched) {
			fnNameStr = fnNameRaw + "/" + fnArity.str();
			nparams = (char)std::stoi(fnArity.str());
		}
		return SFFunctionArity_t({ fnNameRaw, fnNameStr, fnArityStr, getAtomPtr(fnNameStr), getAtomPtr(fnArityStr), nparams });
	}
}
