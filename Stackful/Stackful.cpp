// Stackful.cpp : Defines the entry point for the console application.
//

#ifdef _WIN32
#include "stdafx.h"
#endif
#include <map>
#include <iostream>

#include "include/sftypes.hpp"

typedef std::map<SFInteger_t, std::string> atomsById_t;
typedef std::map<std::string, SFInteger_t> atomsByName_t;

class SFState {
public:
	SFState() {
		// Initialize default atoms
		getAtom("nil");
		getAtom("false");
		getAtom("true");
	}
	virtual ~SFState() {
	}
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
protected:
	SFList ops;
	size_t atomCounter = 0;
	atomsById_t atomsById;
	atomsByName_t atomsByName;
};

int main()
{
	SFInteger i1(1);
	SFInteger i2(5);
	SFInteger i3(i1 ^ i2);
	SFList l1;

	l1.push_back(i1);
	l1.push_back(i2);
	l1.push_back(i3);

	SFList l2(l1);
	SFList l3(l1 + l2);
	SFList l4(l1);

	l4.push_back(l1);

	SFState state;

	std::cout << l4.str() << std::endl;
    return 0;
}

