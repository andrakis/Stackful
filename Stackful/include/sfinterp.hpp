#pragma once

#include "sfextypes.hpp"

namespace stackful {
	class SFInterpreter {
	public:
		SFInterpreter() : depth(0), functionCalls(0), debugMode(true) { }
		~SFInterpreter() { }
		SFLiteral_p run(SFLiteral_p chain_p) throw(std::runtime_error);
		SFInteger_t getDepth() const { return depth; }
		SFInteger_t getFunctionCalls() const { return functionCalls; }
		bool getDebug() { return debugMode; }
		SFLiteral_p invokeFunctionCall(SFLiteral_p fndef, const SFFnCallSignature_t &call);
		std::string inspectObject(const SFBasicList &obj) const;
	protected:
		SFInteger_t depth;
		SFInteger_t functionCalls;
		bool debugMode;
		SFLiteral_p doFunctionCall(SFLiteral_p chain, SFFunctionCall *i);
		SFLiteral_p getParamValue(SFLiteral_p chain_p, SFLiteral_p p);
	};
}

void interp_test();