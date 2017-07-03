#pragma once

#include <map>
#include <string>

size_t getAtom(std::string name);
std::string getAtom(size_t id);
extern size_t atomNil, atomFalse, atomTrue;
