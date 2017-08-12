#pragma once

#include "../stdafx.h"

#include "sfextypes.hpp"
#include "sfbuiltins.hpp"
#include "sffndef.hpp"

namespace stackful {
	typedef unsigned long SFStatCounter;
	typedef struct SFStats_s {
		SFStats_s() : function_calls(0), resolutions(0), topmost_hits(0), topmost_misses(0) {
		}
		SFStatCounter function_calls,
		              resolutions,
		              topmost_hits,
		              topmost_misses;
		std::string str() const {
			std::string s = "FnCalls: " + std::to_string(function_calls) + ", ";
			            s+= "Resolutions: " + std::to_string(resolutions) + ", ";
			            s+= "Topmost (Hits: " + std::to_string(topmost_hits) +
			                         ", Misses: " + std::to_string(topmost_misses) + ")";
			return s;
		}
	} SFStats_t;
	class SFInterpreter {
	public:
		SFInterpreter() : functionCalls(0), depth(0), debugMode(true) { }
		~SFInterpreter() { }
		SFLiteral_p run(SFLiteral_p chain_p) throw(std::runtime_error);
		SFInteger_t getDepth() const { return depth; }
		SFInteger_t getFunctionCalls() const { return functionCalls; }
		bool getDebug() { return debugMode; }
		SFLiteral_p invokeFunctionCall(SFLiteral_p fndef, const SFFnCallSignature_t &call);
		std::string inspectObject(const SFBasicList &obj) const;
		std::string inspectObject(const SFBasicList &obj, const std::string &pre, const std::string &post) const;
		std::string getStats() const { return stats.str(); };
	protected:
		SFStats_t stats;
		SFInteger_t functionCalls;
		int depth;
		bool debugMode;

		SFLiteral_p doFunctionCall(SFLiteral_p chain, const SFFunctionCall &i);
		SFLiteral_p getParamValue(SFLiteral_p chain_p, SFLiteral_p p);
		SFLiteral_p getFnDefFromClosure(const SFClosure &closure, const SFFunctionCall &i, const SFFunctionArity_t &details) const;
		SFLiteral_p getFnDef(const SFOpChain &chain, const SFFunctionCall &i, const SFFunctionArity_t &details) throw(std::runtime_error);
	};
}

void interp_test();
