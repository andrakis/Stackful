#include "../stdafx.h"

#include "../../contrib/json.hpp"
#include <map>
#include <regex>
#include <string>

#include "../include/sfextypes.hpp"
#include "../include/sffndef.hpp"

#include "../include/sfparser.hpp"

namespace stackful {
	ArityBuiltins_t ArityBuiltins = {
		{"print", "*"},
		{"and", "*"},
		{"or", "*"},
		{"+", "*"},
		{"++", "*"},
		{"-", "*"},
		{"/", "*"},
		{"\\", "*"},
		{"list", "*"},
		{"flatten", "*"},
		{"call", "*"},
		{"to-string", "*"},
		{"export", "*"},
		{"export-global", "*"},
		{"invoke", "*"},
		{"dict", "*"},
	};

	SupportedEscapes_t SupportedEscapes = {
		{"n", "\n"},
		{"r", "\r"},
		{"t", "\t"},
		{"\\", "\\\\"},
	};

	VarRefFunctions_t VarRefFunctions = {
		"get", "set", "var"
	};

	ExTableStr_t ExTableStr = {
		{"EX_LITERAL", EX_LITERAL},
		{"EX_OPCHAIN", EX_OPCHAIN},
		{"EX_FUNCTIONCALL", EX_FUNCTIONCALL},
		{"EX_NUMBER", EX_NUMBER},
		{"EX_ATOM", EX_ATOM},
		{"EX_VARIABLE", EX_VARIABLE},
		{"EX_STRING_CHARACTER", EX_STRING_CHARACTER},
		{"EX_STRING_SINGLE", EX_STRING_SINGLE},
		{"EX_STRING_DOUBLE", EX_STRING_DOUBLE},
		{"EX_PARAM_SEPARATOR", EX_PARAM_SEPARATOR},
		{"EX_CALL_END", EX_CALL_END},
		{"EX_OPCHAIN_END", EX_OPCHAIN_END},
		{"EX_COMMENT", EX_COMMENT},
		{"EX_COMPILED", EX_COMPILED},
		{"EX_FUNCTION_MARKER", EX_FUNCTION_MARKER},
		{"EX_FUNCTION_PARAM", EX_FUNCTION_PARAM},
		{"EX_FUNCTION_PARAM_SEP", EX_FUNCTION_PARAM_SEP},
		{"EX_FUNCTION_BODY", EX_FUNCTION_BODY},
	};

	ExTableExpect_t ExTableExpect = {
		{EX_LITERAL, "EX_LITERAL"},
		{EX_OPCHAIN, "EX_OPCHAIN"},
		{EX_FUNCTIONCALL, "EX_FUNCTIONCALL"},
		{EX_NUMBER, "EX_NUMBER"},
		{EX_ATOM, "EX_ATOM"},
		{EX_VARIABLE, "EX_VARIABLE"},
		{EX_STRING_CHARACTER, "EX_STRING_CHARACTER"},
		{EX_STRING_SINGLE, "EX_STRING_SINGLE"},
		{EX_STRING_DOUBLE, "EX_STRING_DOUBLE"},
		{EX_PARAM_SEPARATOR, "EX_PARAM_SEPARATOR"},
		{EX_CALL_END, "EX_CALL_END"},
		{EX_OPCHAIN_END, "EX_OPCHAIN_END"},
		{EX_COMMENT, "EX_COMMENT"},
		{EX_COMPILED, "EX_COMPILED"},
		{EX_FUNCTION_MARKER, "EX_FUNCTION_MARKER"},
		{EX_FUNCTION_PARAM, "EX_FUNCTION_PARAM"},
		{EX_FUNCTION_PARAM_SEP, "EX_FUNCTION_PARAM_SEP"},
		{EX_FUNCTION_BODY, "EX_FUNCTION_BODY"},
	};

	int GET_EX(const std::string &id) {
		ExTableStr_t::const_iterator it = ExTableStr.find(id);
		if (it == ExTableStr.end())
			return 0;
		return it->second;
	}

	std::string GET_EX(const SFParserExpect val) {
		ExTableExpect_t::const_iterator it = ExTableExpect.begin();
		std::stringstream ss;
		bool first = true;
		for (; it != ExTableExpect.end(); ++it) {
			const SFParserExpect n = it->first;
			if ((val & n) != 0) {
				if (first)
					first = false;
				else
					ss << " | ";
				ss << it->second;
			}
		}
		return ss.str();
	}

	SFParser::SFParser()
	{
	}


	SFParser::~SFParser()
	{
	}
}
