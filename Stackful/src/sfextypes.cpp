#include "../stdafx.h"

#include <exception>
#include <sstream>
#include <string>

#include "../include/sfatoms.hpp"
#include "../include/sfextypes.hpp"

namespace stackful {
	ExtendedType getExtendedType(const SFLiteral_p &p_p) {
		const SFLiteral *p = p_p.get();
		return getExtendedType(p);
	}

	ExtendedType getExtendedType(const SFLiteral *p) {
		if (p->isExtended()) {
			const SFExtended *p_e = p->ExtClass();
			return p_e->getExtendedType();
		} else {
			switch (p->getType()) {
				case Basic_Integer:
					return __Basic_Integer;
				case Basic_List:
					return __Basic_List;
				default: return __Unknown;
			}
		}
	}

	std::string getReadableName(ExtendedType t) {
		switch(t) {
			case Atom: return "Atom";
			case Integer: return "Integer";
			case Float: return "Float";
			case String: return "String";
			case List: return "List";
			case Dictionary: return "Dictionary";
			case FunctionCall: return "FunctionCall";
			case OpChain: return "OpChain";
			case Closure: return "Closure";
			case FunctionDefinition: return "FunctionDefinition";
			case FunctionDefinitionNative: return "FunctionDefinitionNantive";
			default: return "INVALID_TYPE";
		}
	}

	SFExtended::~SFExtended() {
#ifdef _LIFETIME
		auto now = std::chrono::system_clock::now();
		std::stringstream ss;
		ss << "A ";
		ss << getReadableName(this->getExtendedType());
		ss << " has died (lifetime: ";
		ss << std::chrono::duration_cast<std::chrono::milliseconds>(now - this->start).count();
		ss << "ms)";
		std::cerr << ss.str() << std::endl;
#endif
		this->clear();
	}

	SFExtended* SFLiteral::ExtClass() {
		if (this->type != Basic_List)
			throw std::runtime_error("Not a list");
		return static_cast<SFExtended*>(this);
	}
	const SFExtended* SFLiteral::ExtClass() const {
		if (this->type != Basic_List)
			throw std::runtime_error("Not a list");
		return static_cast<const SFExtended*>(this);
	}

	SFOpChain *toOpChain(const SFLiteral_p &opchain_p) {
		return static_cast<SFOpChain*>(opchain_p.get());
	}
	SFClosure *toClosure(const SFLiteral_p &closure_p) {
		return static_cast<SFClosure*>(closure_p.get());
	}

	SFLiteral_p tolist(const std::string &str) {
		SFBasicList *l = new SFBasicList();
		std::string::const_iterator it = str.begin();
		for (; it != str.end(); ++it) {
			// Generally a char
			auto curr = *it;
			l->push_back(curr);
		}
		return SFLiteral_p(l);
	}

	SFBasicList sfvariable(const SFInteger_t type, SFLiteral_p value) {
		SFBasicList v;
		v.push_back(type);
		v.push_back(value);
		return v;
	}

	SFBasicList sfvar(const std::string &str) {
		return SFString(str);
	}

	SFBasicList sfvarfloat(const double value) {
		return SFFloat(value);
	}

	SFBasicList sfvar(const SFInteger_t value) {
		return sfvariable(Integer, SFLiteral_p(new SFBasicInteger(value)));
	}

	SFBasicList sfatom(const std::string &s) {
		return sfvariable(Atom, SFLiteral_p(new SFBasicInteger(getAtom(s))));
	}

	ExtendedType identifyLiteral(const SFBasicList &l) {
		if (l.size() != 2)
			throw std::runtime_error(std::string("Invalid literal: ") + l.str());
		SFBasicInteger *firstElement = l[0]->IntegerClass();
		return (ExtendedType)firstElement->getValue();
	}

	std::string varstr(const SFBasicList &l) {
		return l.str();
	}

	stackful::SFFunctionCall::SFFunctionCall(const std::string &fn)
	: SFList(FunctionCall), details(getFunctionArity(fn, 0))
	{
		this->push_back(getAtomPtr(details.nameArityStr));
		this->push_back(new SFList());
	}

	SFFunctionCall::SFFunctionCall(const std::string &fn, SFLiteral_p p1)
	: SFList(FunctionCall), details(getFunctionArity(fn, 1))
	{
		this->push_back(getAtomPtr(details.nameArityStr));
		SFList *params = new SFList();
		params->push_back(p1);
		this->push_back(SFLiteral_p(params));
	}

	SFFunctionCall::SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2)
	: SFList(FunctionCall), details(getFunctionArity(fn, 2))
	{
		this->push_back(getAtomPtr(details.nameArityStr));
		SFList *params = new SFList();
		params->push_back(p1);
		params->push_back(p2);
		this->push_back(SFLiteral_p(params));
	}

	SFFunctionCall::SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2, SFLiteral_p p3)
	: SFList(FunctionCall), details(getFunctionArity(fn, 3))
	{
		this->push_back(getAtomPtr(details.nameArityStr));
		SFList *params = new SFList();
		params->push_back(p1);
		params->push_back(p2);
		params->push_back(p3);
		this->push_back(SFLiteral_p(params));
	}

	SFFunctionCall::SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2, SFLiteral_p p3, SFLiteral_p p4)
	: SFList(FunctionCall), details(getFunctionArity(fn, 0))
	{
		this->push_back(getAtomPtr(details.nameArityStr));
		SFList *params = new SFList();
		params->push_back(p1);
		params->push_back(p2);
		params->push_back(p3);
		params->push_back(p4);
		this->push_back(SFLiteral_p(params));
	}

	std::string SFFunctionCall::_str() const {
		std::stringstream ss;
		ss << "{FunctionCall: ";
		ss << this->getFunction()->ExtClass()->extLiteral();
		ss << this->getArguments()->ExtClass()->extLiteral();
		ss << "}";
		return ss.str();
	}

	std::string SFOpChain::_str() const {
		std::stringstream ss;
		bool first = true;
		ss << "{OpChain: ";
		//ss << this->getClosureObject()->str();
		ss << " [";
		SFOpChain::iterator it = this->begin();
		for (; it != this->end(); ++it) {
			SFLiteral_p item = *it;
			if (first)
				first = false;
			else
				ss << ", ";
			if (item->isExtended())
				ss << item->ExtClass()->extLiteral();
			else
				ss << item->str();
		}
		ss << "]}";
		return ss.str();
	}

	SFInteger_t atomPtrsCounter = 0;
	atomPtrsById_t atomPtrsById;
	SFLiteral_p getAtomPtr(SFInteger_t id) {
		atomPtrsById_t::iterator it = atomPtrsById.find(id);
		if (it != atomPtrsById.end()) {
			return it->second;
		}

		SFAtom *atom = new SFAtom(id);
		SFLiteral_p atom_p(atom);
		atomPtrsById.emplace(id, atom_p);
		return atom_p;
	}
	SFLiteral_p getAtomPtr(const std::string &name) {
		return getAtomPtr(getAtom(name));
	}

	SFInteger* operator+(const SFInteger &a, const SFInteger &b) {
		return new SFInteger(a.getRawValue() + b.getRawValue());
	}
	SFInteger* operator-(const SFInteger &a, const SFInteger &b) {
		return new SFInteger(a.getRawValue() - b.getRawValue());
	}
	SFInteger* operator/(const SFInteger &a, const SFInteger &b) {
		return new SFInteger(a.getRawValue() / b.getRawValue());
	}
	SFInteger* operator*(const SFInteger &a, const SFInteger &b) {
		return new SFInteger(a.getRawValue() * b.getRawValue());
	}
	SFInteger* operator|(const SFInteger &a, const SFInteger &b) {
		return new SFInteger(a.getRawValue() | b.getRawValue());
	}
	SFInteger* operator^(const SFInteger &a, const SFInteger &b) {
		return new SFInteger(a.getRawValue() ^ b.getRawValue());
	}
	SFInteger* operator!(const SFInteger &a) {
		return new SFInteger(!a.getRawValue());
	}
	SFInteger* operator<<(const SFInteger &a, const SFInteger &b) {
		return new SFInteger(a.getRawValue() << b.getRawValue());
	}
	SFInteger* operator>>(const SFInteger &a, const SFInteger &b) {
		return new SFInteger(a.getRawValue() >> b.getRawValue());
	}

	SFFloat* operator+(const SFFloat &a, const SFFloat &b) {
		return new SFFloat(a.getRawValue() + b.getRawValue());
	}
	SFFloat* operator-(const SFFloat &a, const SFFloat &b) {
		return new SFFloat(a.getRawValue() - b.getRawValue());
	}
	SFFloat* operator/(const SFFloat &a, const SFFloat &b) {
		return new SFFloat(a.getRawValue() / b.getRawValue());
	}
	SFFloat* operator*(const SFFloat &a, const SFFloat &b) {
		return new SFFloat(a.getRawValue() * b.getRawValue());
	}

	SFInteger::operator SFFloat*() const {
		return new SFFloat((double)this->getRawValue());
	}
	SFFloat* SFInteger::getFloatValue() const {
		return this->operator stackful::SFFloat*();
	}
	SFInteger::operator SFString*() const {
		return new SFString(this->str());
	}

	SFFloat::operator SFInteger*() const {
		return new SFInteger((SFInteger_t)this->getRawValue());
	}
	SFFloat::operator SFString*() const {
		return new SFString(str());
	}

	SFList* operator+(const SFList &a, const SFList &b) {
		SFList *tmp = new SFList(a);
		tmp->ShallowCopy(b);
		return tmp;
	}

	SFExtended* operator+(const SFExtended &a, const SFExtended &b) throw(std::runtime_error) {
		SFLiteral_p p;
		switch (a.getExtendedType()) {
			case Integer:
			{
				const SFInteger *ia = static_cast<const SFInteger*>(&a);
				const SFInteger *ib;
				if (b.getExtendedType() == Integer)
					ib = static_cast<const SFInteger*>(&b);
				else if (b.getExtendedType() == Float) {
					const SFFloat *fb = static_cast<const SFFloat*>(&b);
					SFInteger *tmp = fb->operator stackful::SFInteger *();
					p = SFLiteral_p(tmp);
					ib = tmp;
				}
				else
					throw std::runtime_error("Cannot add given type to integer");
#ifdef _AUTO_PROMOTE
				SFInteger_t iia = ia->getRawValue();
				SFInteger_t iib = ib->getRawValue();
				if (abs(iia) != 0 && abs(iib) != 0 && abs(iia) / abs(iib) == 0) {
					SFFloat *fa = ia->getFloatValue();
					SFLiteral_p fa_p(fa);

					if (b.getExtendedType() == Float) {
						const SFFloat *fb = static_cast<const SFFloat*>(&b);
						return *fa + *fb;
					}

					SFFloat *fb = ib->getFloatValue();
					SFLiteral_p fb_p(fb);
					return *fa + *fb;
				}
#endif
				return *ia + *ib;
			}
			case Float:
			{
				const SFFloat *fa = static_cast<const SFFloat*>(&a);
				const SFFloat *fb;
				if (b.getExtendedType() == Float)
					fb = static_cast<const SFFloat*>(&b);
				else if (b.getExtendedType() == Integer) {
					const SFInteger *ib = static_cast<const SFInteger*>(&b);
					SFFloat *tmp = ib->operator stackful::SFFloat *();
					p = SFLiteral_p(tmp);
					fb = tmp;
				}
				else
					throw std::runtime_error("Cannot add given type to float");
				return *fa + *fb;
			}
			case String:
			{
				const SFString *sa = static_cast<const SFString*>(&a);
				const SFString *sb;
				if (b.getExtendedType() == String)
					sb = static_cast<const SFString*>(&b);
				else {
					SFString *tmp = new SFString(b.str());
					p = SFLiteral_p(tmp);
					sb = tmp;
				}
				return *sa + *sb;
			}
			case List:
			{
				const SFList *la = static_cast<const SFList*>(&a);
				const SFList *lb;
				if (b.getExtendedType() == List)
					lb = static_cast<const SFList*>(&b);
				else {
					SFList *tmp = new SFList();
					tmp->push_back(b);
					p = SFLiteral_p(tmp);
					lb = tmp;
				}
				return *la + *lb;
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFExtended* operator-(const SFExtended &a, const SFExtended &b) throw(std::runtime_error) {
		SFLiteral_p p;
		switch (a.getExtendedType()) {
			case Integer:
			{
				const SFInteger *ia = static_cast<const SFInteger*>(&a);
				const SFInteger *ib;
				if (b.getExtendedType() == Integer)
					ib = static_cast<const SFInteger*>(&b);
				else if (b.getExtendedType() == Float) {
					const SFFloat *fb = static_cast<const SFFloat*>(&b);
					SFInteger *tmp = fb->operator stackful::SFInteger *();
					p = SFLiteral_p(tmp);
					ib = tmp;
				}  else
					throw std::runtime_error("Cannot subtract given type to integer");
#ifdef _AUTO_PROMOTE
				SFInteger_t iia = ia->getRawValue();
				SFInteger_t iib = ib->getRawValue();
				if (abs(iia) > 0 && abs(iib) > 0 && abs(iia) * abs(iib) <= 0) {
					SFFloat *fa = ia->getFloatValue();
					SFLiteral_p fa_p(fa);

					if (b.getExtendedType() == Float) {
						const SFFloat *fb = static_cast<const SFFloat*>(&b);
						return *fa - *fb;
					}

					SFFloat *fb = ib->getFloatValue();
					SFLiteral_p fb_p(fb);
					return *fa - *fb;
				}
#endif
				return *ia - *ib;
			}
			case Float:
			{
				const SFFloat *fa = static_cast<const SFFloat*>(&a);
				const SFFloat *fb;
				if (b.getExtendedType() == Float)
					fb = static_cast<const SFFloat*>(&b);
				else if (b.getExtendedType() == Integer) {
					const SFInteger *ib = static_cast<const SFInteger*>(&b);
					SFFloat *tmp = ib->operator stackful::SFFloat *();
					p = SFLiteral_p(tmp);
					fb = tmp;
				} else
					throw std::runtime_error("Cannot subtract given type to float");
				return *fa - *fb;
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFExtended* operator*(const SFExtended &a, const SFExtended &b) throw(std::runtime_error) {
		SFLiteral_p p;
		switch (a.getExtendedType()) {
			case Integer:
			{
				const SFInteger *ia = static_cast<const SFInteger*>(&a);
				const SFInteger *ib;
				if (b.getExtendedType() == Integer)
					ib = static_cast<const SFInteger*>(&b);
				else if (b.getExtendedType() == Float) {
					const SFFloat *fb = static_cast<const SFFloat*>(&b);
					SFInteger *tmp = fb->operator stackful::SFInteger *();
					p = SFLiteral_p(tmp);
					ib = tmp;
				} else
					throw std::runtime_error("Cannot multiply given type to integer");
#ifdef _AUTO_PROMOTE
				SFInteger_t iia = ia->getRawValue();
				SFInteger_t iib = ib->getRawValue();
				if (abs(iia) != 0 && abs(iib) != 0 && abs(iia) * abs(iib) <= 0) {
					SFFloat *fa = ia->getFloatValue();
					SFLiteral_p fa_p(fa);

					if (b.getExtendedType() == Float) {
						const SFFloat *fb = static_cast<const SFFloat*>(&b);
						return *fa * *fb;
					}

					SFFloat *fb = ib->getFloatValue();
					SFLiteral_p fb_p(fb);
					return *fa * *fb;
				}
#endif
				return *ia * *ib;
			}
			case Float:
			{
				const SFFloat *fa = static_cast<const SFFloat*>(&a);
				const SFFloat *fb;
				if (b.getExtendedType() == Float)
					fb = static_cast<const SFFloat*>(&b);
				else if (b.getExtendedType() == Integer) {
					const SFInteger *ib = static_cast<const SFInteger*>(&b);
					SFFloat *tmp = ib->operator stackful::SFFloat *();
					p = SFLiteral_p(tmp);
					fb = tmp;
				} else
					throw std::runtime_error("Cannot multiply given type to float");
				return *fa * *fb;
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFExtended* operator/(const SFExtended &a, const SFExtended &b) throw(std::runtime_error) {
		SFLiteral_p p;
		switch (a.getExtendedType()) {
			case Integer:
			{
				const SFInteger *ia = static_cast<const SFInteger*>(&a);
				const SFInteger *ib;
				if (b.getExtendedType() == Integer)
					ib = static_cast<const SFInteger*>(&b);
				else if (b.getExtendedType() == Float) {
					const SFFloat *fb = static_cast<const SFFloat*>(&b);
					SFInteger *tmp = fb->operator stackful::SFInteger *();
					p = SFLiteral_p(tmp);
					ib = tmp;
				} else
					throw std::runtime_error("Cannot divide given type to integer");
#ifdef _AUTO_PROMOTE
				SFInteger_t iia = ia->getRawValue();
				SFInteger_t iib = ib->getRawValue();
				if (abs(iia) != 0 && abs(iib) != 0 && abs(iia) / abs(iib) == 0) {
					SFFloat *fa = ia->getFloatValue();
					SFFloat *fb = ib->getFloatValue();
					SFLiteral_p fa_p(fa);
					SFLiteral_p fb_p(fb);
					return *fa / *fb;
				}
#endif
				return *ia / *ib;
			}
			case Float:
			{
				const SFFloat *fa = static_cast<const SFFloat*>(&a);
				const SFFloat *fb;
				if (b.getExtendedType() == Float)
					fb = static_cast<const SFFloat*>(&b);
				else if (b.getExtendedType() == Integer) {
					const SFInteger *ib = static_cast<const SFInteger*>(&b);
					SFFloat *tmp = ib->operator stackful::SFFloat *();
					p = SFLiteral_p(tmp);
					fb = tmp;
				} else
					throw std::runtime_error("Cannot divide given type to float");
				return *fa / *fb;
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFExtended* operator|(const SFExtended &a, const SFExtended &b) throw(std::runtime_error) {
		SFLiteral_p p;
		switch (a.getExtendedType()) {
			case Integer:
			{
				const SFInteger *ia = static_cast<const SFInteger*>(&a);
				const SFInteger *ib;
				if (b.getExtendedType() == Integer)
					ib = static_cast<const SFInteger*>(&b);
				else if (b.getExtendedType() == Float) {
					const SFFloat *fb = static_cast<const SFFloat*>(&b);
					SFInteger *tmp = fb->operator stackful::SFInteger *();
					p = SFLiteral_p(tmp);
					ib = tmp;
				} else
					throw std::runtime_error("Cannot binary OR given type to integer");
				return *ia | *ib;
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFExtended* operator^(const SFExtended &a, const SFExtended &b) throw(std::runtime_error) {
		SFLiteral_p p;
		switch (a.getExtendedType()) {
			case Integer:
			{
				const SFInteger *ia = static_cast<const SFInteger*>(&a);
				const SFInteger *ib;
				if (b.getExtendedType() == Integer)
					ib = static_cast<const SFInteger*>(&b);
				else if (b.getExtendedType() == Float) {
					const SFFloat *fb = static_cast<const SFFloat*>(&b);
					SFInteger *tmp = fb->operator stackful::SFInteger *();
					p = SFLiteral_p(tmp);
					ib = tmp;
				} else
					throw std::runtime_error("Cannot binary XOR given type to integer");
				return *ia ^ *ib;
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFExtended* operator&(const SFExtended &a, const SFExtended &b) throw(std::runtime_error) {
		SFLiteral_p p;
		switch (a.getExtendedType()) {
			case Integer:
			{
				const SFInteger *ia = static_cast<const SFInteger*>(&a);
				const SFInteger *ib;
				if (b.getExtendedType() == Integer)
					ib = static_cast<const SFInteger*>(&b);
				else if (b.getExtendedType() == Float) {
					const SFFloat *fb = static_cast<const SFFloat*>(&b);
					SFInteger *tmp = fb->operator stackful::SFInteger *();
					p = SFLiteral_p(tmp);
					ib = tmp;
				} else
					throw std::runtime_error("Cannot binary AND given type to integer");
				return *ia & *ib;
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFExtended* operator%(const SFExtended &a, const SFExtended &b) throw(std::runtime_error) {
		SFLiteral_p p;
		switch (a.getExtendedType()) {
			case Integer:
			{
				const SFInteger *ia = static_cast<const SFInteger*>(&a);
				const SFInteger *ib;
				if (b.getExtendedType() == Integer)
					ib = static_cast<const SFInteger*>(&b);
				else if (b.getExtendedType() == Float) {
					const SFFloat *fb = static_cast<const SFFloat*>(&b);
					SFInteger *tmp = fb->operator stackful::SFInteger *();
					p = SFLiteral_p(tmp);
					ib = tmp;
				} else
					throw std::runtime_error("Cannot modulo given type to integer");
				return *ia % *ib;
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFExtended* operator<<(const SFExtended &a, const SFExtended &b) throw(std::runtime_error) {
		SFLiteral_p p;
		switch (a.getExtendedType()) {
			case Integer:
			{
				const SFInteger *ia = static_cast<const SFInteger*>(&a);
				const SFInteger *ib;
				if (b.getExtendedType() == Integer)
					ib = static_cast<const SFInteger*>(&b);
				else if (b.getExtendedType() == Float) {
					const SFFloat *fb = static_cast<const SFFloat*>(&b);
					SFInteger *tmp = fb->operator stackful::SFInteger *();
					p = SFLiteral_p(tmp);
					ib = tmp;
				} else
					throw std::runtime_error("Cannot shift left given type to integer");
				return *ia << *ib;
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFExtended* operator>>(const SFExtended &a, const SFExtended &b) throw(std::runtime_error) {
		SFLiteral_p p;
		switch (a.getExtendedType()) {
			case Integer:
			{
				const SFInteger *ia = static_cast<const SFInteger*>(&a);
				const SFInteger *ib;
				if (b.getExtendedType() == Integer)
					ib = static_cast<const SFInteger*>(&b);
				else if (b.getExtendedType() == Float) {
					const SFFloat *fb = static_cast<const SFFloat*>(&b);
					SFInteger *tmp = fb->operator stackful::SFInteger *();
					p = SFLiteral_p(tmp);
					ib = tmp;
				} else
					throw std::runtime_error("Cannot shift right given type to integer");
				return *ia >> *ib;
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}

	bool operator<(const SFExtended &a, const SFExtended &b) throw(std::runtime_error) {
		SFLiteral_p p;
		switch (a.getExtendedType()) {
			case Integer:
			{
				const SFInteger *ia = static_cast<const SFInteger*>(&a);
				const SFInteger *ib;
				if (b.getExtendedType() == Integer)
					ib = static_cast<const SFInteger*>(&b);
				else if (b.getExtendedType() == Float) {
					const SFFloat *fb = static_cast<const SFFloat*>(&b);
					SFInteger *tmp = fb->operator stackful::SFInteger *();
					p = SFLiteral_p(tmp);
					ib = tmp;
				} else
					throw std::runtime_error("Cannot compare given type to integer");
				return ia->getRawValue() < ib->getRawValue();
			}
			case Float:
			{
				const SFFloat *fa = static_cast<const SFFloat*>(&a);
				const SFFloat *fb;
				if (b.getExtendedType() == Float)
					fb = static_cast<const SFFloat*>(&b);
				else if (b.getExtendedType() == Integer) {
					const SFInteger *ib = static_cast<const SFInteger*>(&b);
					SFFloat *tmp = ib->operator stackful::SFFloat *();
					p = SFLiteral_p(tmp);
					fb = tmp;
				} else
					throw std::runtime_error("Cannot compare given type to float");
				return fa->getRawValue() < fb->getRawValue();
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	bool operator>(const SFExtended &a, const SFExtended &b) throw(std::runtime_error) {
		SFLiteral_p p;
		switch (a.getExtendedType()) {
			case Integer:
			{
				const SFInteger *ia = static_cast<const SFInteger*>(&a);
				const SFInteger *ib;
				if (b.getExtendedType() == Integer)
					ib = static_cast<const SFInteger*>(&b);
				else if (b.getExtendedType() == Float) {
					const SFFloat *fb = static_cast<const SFFloat*>(&b);
					SFInteger *tmp = fb->operator stackful::SFInteger *();
					p = SFLiteral_p(tmp);
					ib = tmp;
				} else
					throw std::runtime_error("Cannot compare given type to integer");
				return ia->getRawValue() > ib->getRawValue();
			}
			case Float:
			{
				const SFFloat *fa = static_cast<const SFFloat*>(&a);
				const SFFloat *fb;
				if (b.getExtendedType() == Float)
					fb = static_cast<const SFFloat*>(&b);
				else if (b.getExtendedType() == Integer) {
					const SFInteger *ib = static_cast<const SFInteger*>(&b);
					SFFloat *tmp = ib->operator stackful::SFFloat *();
					p = SFLiteral_p(tmp);
					fb = tmp;
				} else
					throw std::runtime_error("Cannot compare given type to float");
				return fa->getRawValue() > fb->getRawValue();
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	bool operator<=(const SFExtended &a, const SFExtended &b) throw(std::runtime_error) {
		SFLiteral_p p;
		switch (a.getExtendedType()) {
			case Integer:
			{
				const SFInteger *ia = static_cast<const SFInteger*>(&a);
				const SFInteger *ib;
				if (b.getExtendedType() == Integer)
					ib = static_cast<const SFInteger*>(&b);
				else if (b.getExtendedType() == Float) {
					const SFFloat *fb = static_cast<const SFFloat*>(&b);
					SFInteger *tmp = fb->operator stackful::SFInteger *();
					p = SFLiteral_p(tmp);
					ib = tmp;
				} else
					throw std::runtime_error("Cannot compare given type to integer");
				return ia->getRawValue() <= ib->getRawValue();
			}
			case Float:
			{
				const SFFloat *fa = static_cast<const SFFloat*>(&a);
				const SFFloat *fb;
				if (b.getExtendedType() == Float)
					fb = static_cast<const SFFloat*>(&b);
				else if (b.getExtendedType() == Integer) {
					const SFInteger *ib = static_cast<const SFInteger*>(&b);
					SFFloat *tmp = ib->operator stackful::SFFloat *();
					p = SFLiteral_p(tmp);
					fb = tmp;
				} else
					throw std::runtime_error("Cannot compare given type to float");
				return fa->getRawValue() <= fb->getRawValue();
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	bool operator>=(const SFExtended &a, const SFExtended &b) throw(std::runtime_error)  {
		SFLiteral_p p;
		switch (a.getExtendedType()) {
			case Integer:
			{
				const SFInteger *ia = static_cast<const SFInteger*>(&a);
				const SFInteger *ib;
				if (b.getExtendedType() == Integer)
					ib = static_cast<const SFInteger*>(&b);
				else if (b.getExtendedType() == Float) {
					const SFFloat *fb = static_cast<const SFFloat*>(&b);
					SFInteger *tmp = fb->operator stackful::SFInteger *();
					p = SFLiteral_p(tmp);
					ib = tmp;
				} else
					throw std::runtime_error("Cannot compare given type to integer");
				return ia->getRawValue() >= ib->getRawValue();
			}
			case Float:
			{
				const SFFloat *fa = static_cast<const SFFloat*>(&a);
				const SFFloat *fb;
				if (b.getExtendedType() == Float)
					fb = static_cast<const SFFloat*>(&b);
				else if (b.getExtendedType() == Integer) {
					const SFInteger *ib = static_cast<const SFInteger*>(&b);
					SFFloat *tmp = ib->operator stackful::SFFloat *();
					p = SFLiteral_p(tmp);
					fb = tmp;
				} else
					throw std::runtime_error("Cannot compare given type to float");
				return fa->getRawValue() >= fb->getRawValue();
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
}
