#pragma once

#include <map>
#include <string>

#include "sftypes.hpp"

namespace stackful {

	SFInteger_t getAtom(std::string name);
	std::string getAtom(SFInteger_t id);
	extern SFInteger_t atomNilId, atomFalseId, atomTrueId;
	extern SFLiteral_p atomNil, atomFalse, atomTrue;

}