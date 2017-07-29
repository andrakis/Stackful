#pragma once

#include <map>
#include <string>

#include "sftypes.hpp"
#include "sfsharedtypes.hpp"

namespace stackful {

	SFInteger_t getAtom(std::string name);
	std::string getAtom(SFInteger_t id);
	extern SFInteger_t atomNilId, atomFalseId, atomTrueId, atomMissingId;
	extern SFInteger_t atomCounter;
	extern atomsById_t atomsById;
	extern atomsByName_t atomsByName;
}