#pragma once

#include "sfbuiltins.hpp"
#include "sfsharedtypes.hpp"

namespace stackful {
	// Generic class (protected constructor) for function definitions
	class SFFunctionDefinitionBase : public SFExtended {
	public:
		SFFnDefName getName() const { return name; }
		SFFnDefArgs getArgs() const { return args; }
		virtual SFLiteral_p invoke(SFBuiltinSignature_t args) = 0;
		std::string extLiteral() const { return _str(); }
	protected:
		SFFunctionDefinitionBase(ExtendedType type, SFFnDefName _name, SFFnDefArgs _args)
		: SFExtended(type), name(_name), args(_args) {
		}
		SFFnDefName name, readable_name;
		SFFnDefArgs args;
		std::string _str() const {
			return "FnDefNative";
		}
	};

	// A function defintion which uses a native callback.
	// That is, one in the host language (C++). This is used for builtins.
	class SFFunctionDefinitionNative : public SFFunctionDefinitionBase {
	public:
		SFFunctionDefinitionNative(SFFnDefName _name, SFFnDefArgs _args, SFBuiltin_f _body);
		SFLiteral_p invoke(SFBuiltinSignature_t args) {
			return nullptr;
		}
	protected:
		SFBuiltin_f body;
	};

	// A function definition which uses Stackful code.
	class SFFunctionDefinition : public SFFunctionDefinitionBase {
	public:
		SFFunctionDefinition(SFOpChain_p parent, SFFnDefName _name, SFFnDefArgs _args, SFOpChain_p body);
		SFLiteral_p invoke(SFBuiltinSignature_t args) {
			return nullptr;
		}
	protected:
	};
}