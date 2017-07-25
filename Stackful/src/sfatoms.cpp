#include <map>
#include <string>
#include "../include/sftypes.hpp"

using namespace stackful;

typedef std::map<SFInteger_t, std::string> atomsById_t;
typedef std::map<std::string, SFInteger_t> atomsByName_t;

SFInteger_t atomCounter = 0;
atomsById_t atomsById;
atomsByName_t atomsByName;

namespace stackful {
	SFInteger_t getAtom(std::string name) {
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

// Initialize default atoms
SFInteger_t atomNil = getAtom("nil"),
	atomFalse = getAtom("false"),
	atomTrue = getAtom("true");

}
