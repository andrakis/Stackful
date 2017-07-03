#include <map>
#include <string>
#include "../include/sftypes.hpp"

typedef std::map<SFInteger_t, std::string> atomsById_t;
typedef std::map<std::string, SFInteger_t> atomsByName_t;

size_t atomCounter = 0;
atomsById_t atomsById;
atomsByName_t atomsByName;
size_t getAtom(std::string name) {
	atomsByName_t::iterator it = atomsByName.find(name);
	if (it != atomsByName.end()) {
		return it->second;
	}

	size_t id = atomCounter++;
	atomsById.emplace(id, name);
	atomsByName.emplace(name, id);
	return id;
}
std::string getAtom(size_t id) {
	return atomsById[id];
}

// Initialize default atoms
size_t atomNil = getAtom("nil"),
	atomFalse = getAtom("false"),
	atomTrue = getAtom("true");
