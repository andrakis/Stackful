#pragma once

#include "sfbuiltins.hpp"
#include "sfsharedtypes.hpp"
#include "sfinterp.hpp"

namespace stackful {
	// Generic class (protected constructor) for function definitions
	class SFFunctionDefinitionBase : public SFExtended {
	public:
		SFFnDefName getName() const { return name; }
		SFFnDefArgs getArgs() const { return args; }
		virtual SFLiteral_p invoke(const SFFnCallSignature_t &args) = 0;
		std::string extLiteral() const { return _str(); }
		bool isScoped() const { return scope != nullptr; }
		SFLiteral_p getScope() const { return scope; }
		void setScope(SFLiteral_p _scope) {
			scope.swap(_scope);
		}
		SFFunctionArity_t getAttributes() const { return attributes; }
		SFFnDefArgsByAtom_t getArgsByAtom() const { return argsByAtom; }
		virtual bool isNative() const = 0;
	protected:
		SFFunctionDefinitionBase(ExtendedType type, const SFFnDefName &_name, const SFFnDefArgs &_args)
		: SFExtended(type), name(_name), args(_args), scope(nullptr),
		  attributes(getFunctionArity(_name, _args.size())) {
			SFFnDefArgs::iterator it = args.begin();
			for (; it != args.end(); ++it) {
				SFInteger_t atomId = getAtom(*it);
				argsByAtom.push_back(atomId);
			}
		}
		SFFnDefName name, readable_name;
		SFFnDefArgs args;
		SFLiteral_p scope;
		SFFunctionArity_t attributes;
		SFFnDefArgsByAtom_t argsByAtom;
		std::string _str() const {
			return "FnDefNative";
		}
	};

	// A function defintion which uses a native callback.
	// That is, one in the host language (C++). This is used for builtins.
	class SFFunctionDefinitionNative : public SFFunctionDefinitionBase {
	public:
		SFFunctionDefinitionNative(const SFFnDefName &_name, const SFFnDefArgs &_args, const SFBuiltin_f &_body);
		SFLiteral_p invoke(const SFFnCallSignature_t &args);
		bool isNative() const { return true; }
	protected:
		SFBuiltin_f body;
	};

	// A function definition which uses Stackful code.
	class SFFunctionDefinition : public SFFunctionDefinitionBase {
	public:
		SFFunctionDefinition(SFLiteral_p parent, const SFFnDefName &_name, const SFFnDefArgs &_args, SFLiteral_p body);
		SFLiteral_p invoke(const SFFnCallSignature_t &args);
		SFLiteral_p getBody() {
			return at(0);
		}
		bool isNative() const { return false; }
	protected:
	};
}
