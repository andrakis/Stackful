#pragma once

#include <map>
#include <string>

#include "sftypes.hpp"

SFInteger_t getAtom(std::string name);
std::string getAtom(SFInteger_t id);
extern SFInteger_t atomNil, atomFalse, atomTrue;
