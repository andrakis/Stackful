// Stackful.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "include/sfatoms.hpp"
#include "include/sfbuiltins.hpp"
#include "include/sftypes.hpp"
#include "include/sfextypes.hpp"
#include "include/sfdebug.hpp"
#include "include/sfinterp.hpp"

using namespace stackful;

int main()
{
	// Initialize default atoms
	atomNil = getAtomPtr("nil");
	atomFalse = getAtomPtr("false");
	atomTrue = getAtomPtr("true");
	atomMissing = getAtomPtr("missing");

#ifdef _DEBUG
	debug.setEnabled(true);
#endif
	interp_test();
#ifdef _PAUSE
	std::string s;
	std::getline(std::cin, s);
#endif
    return 0;
}

