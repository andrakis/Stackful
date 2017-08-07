#include "../stdafx.h"
#include <map>
#include <string>
#include "../include/sftypes.hpp"
#include "../include/sfsharedtypes.hpp"

using namespace stackful;

namespace stackful {
	SFInteger_t atomCounter = 0;
	atomsById_t atomsById;
	atomsByName_t atomsByName;

	SFInteger_t getAtom(const std::string &name) {
		atomsByName_t::iterator it = atomsByName.find(name);
		if (it != atomsByName.end()) {
			return it->second;
		}

		SFInteger_t id = atomCounter++;
		atomsById.emplace(id, name);
		atomsByName.emplace(name, id);
		return id;
	}
	std::string getAtom(SFInteger_t id) {
		return atomsById[id];
	}

	atomsById_t getAtomsById() { return atomsById; }
	atomsByName_t getAtomsByName() { return atomsByName; }
}
