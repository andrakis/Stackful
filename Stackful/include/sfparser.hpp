#pragma once

#include "sfextypes.hpp"

#include <map>
#include <string>

namespace stackful {

	enum SFParserExpect {
		EX_LITERAL = 1 << 0,             // Literal(1  2  "test")
		EX_OPCHAIN = 1 << 1,             // opening OpChain '('
		EX_FUNCTIONCALL = 1 << 2,        // opening FunctionCall '('
		EX_NUMBER = 1 << 3,              // Collect number(whole or float: [0 - 9.] + f ? $)
		EX_ATOM = 1 << 4,                // Collect atom
		EX_VARIABLE = 1 << 5,            // Variables
		EX_STRING_CHARACTER = 1 << 6,    // Collect character
		EX_STRING_SINGLE = 1 << 7,       // Expecting a single quote to end ' 
		EX_STRING_DOUBLE = 1 << 8,       // Expecting a double quote to end " 
		EX_PARAM_SEPARATOR = 1 << 9,     // Expecting a space
		EX_CALL_END = 1 << 10,           // Expected a )  end of call
		EX_OPCHAIN_END = 1 << 11,        // Expect a )  end of opchain
		EX_COMMENT = 1 << 12,            // Comments
		EX_COMPILED = 1 << 13,           // Already compiled
		EX_FUNCTION_MARKER = 1 << 14,    // //     next: Arg1 Arg2 :: (...)
		EX_FUNCTION_PARAM = 1 << 15,     // //     this : Arg1
		EX_FUNCTION_PARAM_SEP = 1 << 16, // //Arg1 this :
		EX_FUNCTION_BODY = 1 << 17       // //Arg1 Arg2  this: ::
	};

	typedef std::map<std::string, std::string> ArityBuiltins_t;
	typedef std::map<std::string, std::string> SupportedEscapes_t;
	typedef std::vector<std::string> VarRefFunctions_t;
	typedef std::map<std::string, SFParserExpect> ExTableStr_t;
	typedef std::map<SFParserExpect, std::string> ExTableExpect_t;

	extern ArityBuiltins_t ArityBuiltins;
	extern SupportedEscapes_t SupportedEscapes;
	extern VarRefFunctions_t VarRefFunctions;
	extern ExTableStr_t ExTableStr;
	extern ExTableExpect_t ExTableExpect;

	std::string GET_EX(const SFParserExpect val);

	class SFParserState {
	public:
	protected:

	};

	class SFParser
	{
	public:
		SFParser();
		~SFParser();
	};
}
