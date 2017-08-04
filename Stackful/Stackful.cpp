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
#ifdef _DEBUG
	debug.setEnabled(true);
#endif
	interp_test();
#ifdef _DEBUG
	debug << "Atom table: " << std::endl;
	atomsByName_t::iterator it = atomsByName.begin();
	for (; it != atomsByName.end(); ++it) {
		debug << "'" << it->first << "' = " << it->second << std::endl;
	}
#endif
    return 0;
}

