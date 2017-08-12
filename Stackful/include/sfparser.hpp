#pragma once

#include "sfatoms.hpp"
#include "sfextypes.hpp"

#include <map>
#include <string>

#include "../include/jslistiterator.hpp"
#include "../../contrib/json.hpp"

using json = nlohmann::json;

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

	typedef JSIterator<SFList*> ListIterator_t;
	typedef std::vector<std::string> SFParserLines_t;
	struct SFParserState {
		SFParserState() :
		ops(new SFList()), ops_p(ops), it(ops_p),
		_current_word(), _expect(EX_OPCHAIN),
		_depth(0), _in_variable(false), _in_atom(false),
		_quote_type(0), _line(1), _character(1),
		_lines(SFParserLines_t()) {
			ops->push_back(new SFList());
		}
		~SFParserState() {
			// ops will be deleted by shared_ptr after 'it' dies
		}
		std::string current_word() const { return _current_word; }
		void current_word(const std::string &val) { _current_word = val; }
		stackful::SFParserExpect expect() const { return _expect; }
		void expect(stackful::SFParserExpect val) { _expect = val; }
		unsigned depth() const { return _depth; }
		void depth(unsigned val) { _depth = val; }
		bool in_variable() const { return _in_variable; }
		void in_variable(bool val) { _in_variable = val; }
		bool in_atom() const { return _in_atom; }
		void in_atom(bool val) { _in_atom = val; }
		char quote_type() const { return _quote_type; }
		void quote_type(char val) { _quote_type = val; }
		unsigned line() const { return _line; }
		void line(unsigned val) { _line = val; }
		unsigned character() const { return _character; }
		void character(unsigned val) { _character = val; }
		SFParserLines_t& lines() { return _lines; }
		SFLiteral_p mapParam(const json &P, const SFOpChain &chain, const std::string &fnName) const;
		SFLiteral_p mapParamInner(const json &P, const SFOpChain &chain, const std::string &fnName) const;
	protected:
		SFList *ops;
		SFLiteral_p ops_p;
		ListIterator_t it;
		std::string _current_word;
		SFParserExpect _expect;
		unsigned _depth;
		bool _in_variable;
		bool _in_atom;
		char _quote_type;
		unsigned _line;
		unsigned _character;
		SFParserLines_t _lines;
	};

	class SFParser
	{
	public:
		SFParser();
		~SFParser();
		SFParserState& getState() {
			return state;
		}
	protected:
		SFParserState state;
	};
}
