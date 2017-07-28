#pragma once

#include "sfextypes.hpp"

namespace stackful {
	class SFInterpreter {
	public:
		SFInterpreter() : depth(0), debugMode(true) { }
		~SFInterpreter() { }
		SFLiteral_p run(SFOpChain_p &chain_p) throw(std::runtime_error);
		SFInteger_t getDepth() const { return depth; }
		bool getDebug() { return debugMode; }
	protected:
		SFInteger_t depth;
		bool debugMode;
		SFLiteral_p do_functioncall(SFOpChain &chain, SFExtended *i) const;
	};
}

void interp_test();