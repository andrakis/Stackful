#pragma once

#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "sfextypes.hpp"

namespace stackful {
	SFBuiltinParams_t params();
	SFBuiltinParams_t params(std::string);
	SFBuiltinParams_t params(std::string, std::string);
	SFBuiltinParams_t params(std::string, std::string, std::string);
	SFBuiltinParams_t params(std::string, std::string, std::string, std::string);

	void setupBuiltins();
	SFBuiltin_f getBuiltin(const SFInteger_t atomId);
	SFBuiltin_f getBuiltin(const std::string &name);
	SFBuiltinDefinitions_t getBuiltinDefinitions();
}
