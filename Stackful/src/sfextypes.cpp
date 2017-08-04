#include "../stdafx.h"

#include <exception>
#include <sstream>
#include <string>

#include "../include/sfatoms.hpp"
#include "../include/sfextypes.hpp"

namespace stackful {
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

	SFOpChain *toOpChain(SFLiteral_p opchain_p) {
		return static_cast<SFOpChain*>(opchain_p.get());
	}
	SFClosure *toClosure(SFLiteral_p closure_p) {
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
		this->push_back(getAtomPtr(fn));
		this->push_back(new SFList());
	}

	SFFunctionCall::SFFunctionCall(const std::string &fn, SFLiteral_p p1)
	: SFList(FunctionCall), details(getFunctionArity(fn, 1))
	{
		this->push_back(getAtomPtr(fn));
		SFList *params = new SFList();
		params->push_back(p1);
		this->push_back(SFLiteral_p(params));
	}

	SFFunctionCall::SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2)
	: SFList(FunctionCall), details(getFunctionArity(fn, 2))
	{
		this->push_back(getAtomPtr(fn));
		SFList *params = new SFList();
		params->push_back(p1);
		params->push_back(p2);
		this->push_back(SFLiteral_p(params));
	}

	SFFunctionCall::SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2, SFLiteral_p p3)
	: SFList(FunctionCall), details(getFunctionArity(fn, 3))
	{
		this->push_back(getAtomPtr(fn));
		SFList *params = new SFList();
		params->push_back(p1);
		params->push_back(p2);
		params->push_back(p3);
		this->push_back(SFLiteral_p(params));
	}

	SFFunctionCall::SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2, SFLiteral_p p3, SFLiteral_p p4)
	: SFList(FunctionCall), details(getFunctionArity(fn, 0))
	{
		this->push_back(getAtomPtr(fn));
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
		ss << "{OpChain: [";
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

	SFExtended* operator+(const SFExtended &a, const SFExtended &b);
	SFExtended* operator-(const SFExtended &a, const SFExtended &b);
	SFExtended* operator*(const SFExtended &a, const SFExtended &b);
	SFExtended* operator/(const SFExtended &a, const SFExtended &b);
	SFExtended* operator|(const SFExtended &a, const SFExtended &b);
	SFExtended* operator^(const SFExtended &a, const SFExtended &b);
	SFExtended* operator%(const SFExtended &a, const SFExtended &b);
	SFExtended* operator<<(const SFExtended &a, const SFExtended &b);
	SFExtended* operator>>(const SFExtended &a, const SFExtended &b);

	SFLiteral* operator+(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.isExtended() && b.isExtended()) {
			const SFExtended *ea = a.ExtClass();
			const SFExtended *eb = b.ExtClass();
			return *ea + *eb;
		}
		switch (a.getType()) {
			case Basic_Integer:
			{
				if (b.getType() != Basic_Integer) {
					throw std::runtime_error("Cannot add a list to an integer");
				}
				const SFBasicInteger *ia = a.IntegerClass();
				const SFBasicInteger *ib = b.IntegerClass();
				return new SFBasicInteger(*ia + *ib);
			}
			case Basic_List:
			{
				const SFBasicList *la = a.ListClass();
				const SFBasicList *lb;
				SFLiteral_p p;
				if (b.getType() == Basic_Integer) {
					const SFBasicInteger *ib = b.IntegerClass();
					SFBasicList *tmp = new SFBasicList();
					tmp->push_back(new SFBasicInteger(ib->getValue()));
					p = SFLiteral_p(tmp);
					lb = tmp;
				} else {
					lb = b.ListClass();
				}
				return new SFBasicList(*la + *lb);
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFLiteral* operator-(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.isExtended() && b.isExtended()) {
			const SFExtended *ea = a.ExtClass();
			const SFExtended *eb = b.ExtClass();
			return *ea - *eb;
		}
		switch (a.getType()) {
			case Basic_Integer:
			{
				if (b.getType() != Basic_Integer) {
					throw std::runtime_error("Cannot subtract a list to an integer");
				}
				const SFBasicInteger *ia = a.IntegerClass();
				const SFBasicInteger *ib = b.IntegerClass();
				return new SFBasicInteger(*ia - *ib);
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFLiteral* operator*(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.isExtended() && b.isExtended()) {
			const SFExtended *ea = a.ExtClass();
			const SFExtended *eb = b.ExtClass();
			return *ea * *eb;
		}
		switch (a.getType()) {
			case Basic_Integer:
			{
				if (b.getType() != Basic_Integer) {
					throw std::runtime_error("Cannot multiply a list to an integer");
				}
				const SFBasicInteger *ia = a.IntegerClass();
				const SFBasicInteger *ib = b.IntegerClass();
				return new SFBasicInteger(*ia * *ib);
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFLiteral* operator/(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.isExtended() && b.isExtended()) {
			const SFExtended *ea = a.ExtClass();
			const SFExtended *eb = b.ExtClass();
			return *ea / *eb;
		}
		switch (a.getType()) {
			case Basic_Integer:
			{
				if (b.getType() != Basic_Integer) {
					throw std::runtime_error("Cannot divide a list to an integer");
				}
				const SFBasicInteger *ia = a.IntegerClass();
				const SFBasicInteger *ib = b.IntegerClass();
				return new SFBasicInteger(*ia / *ib);
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFLiteral* operator|(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.isExtended() && b.isExtended()) {
			const SFExtended *ea = a.ExtClass();
			const SFExtended *eb = b.ExtClass();
			return *ea | *eb;
		}
		switch (a.getType()) {
			case Basic_Integer:
			{
				if (b.getType() != Basic_Integer) {
					throw std::runtime_error("Cannot binary OR a list to an integer");
				}
				const SFBasicInteger *ia = a.IntegerClass();
				const SFBasicInteger *ib = b.IntegerClass();
				return new SFBasicInteger(*ia | *ib);
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFLiteral* operator^(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.isExtended() && b.isExtended()) {
			const SFExtended *ea = a.ExtClass();
			const SFExtended *eb = b.ExtClass();
			return *ea ^ *eb;
		}
		switch (a.getType()) {
			case Basic_Integer:
			{
				if (b.getType() != Basic_Integer) {
					throw std::runtime_error("Cannot binary XOR a list to an integer");
				}
				const SFBasicInteger *ia = a.IntegerClass();
				const SFBasicInteger *ib = b.IntegerClass();
				return new SFBasicInteger(*ia ^ *ib);
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFLiteral* operator&(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.isExtended() && b.isExtended()) {
			const SFExtended *ea = a.ExtClass();
			const SFExtended *eb = b.ExtClass();
			return *ea & *eb;
		}
		switch (a.getType()) {
			case Basic_Integer:
			{
				if (b.getType() != Basic_Integer) {
					throw std::runtime_error("Cannot binary AND a list to an integer");
				}
				const SFBasicInteger *ia = a.IntegerClass();
				const SFBasicInteger *ib = b.IntegerClass();
				return new SFBasicInteger(*ia & *ib);
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFLiteral* operator%(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.isExtended() && b.isExtended()) {
			const SFExtended *ea = a.ExtClass();
			const SFExtended *eb = b.ExtClass();
			return *ea % *eb;
		}
		switch (a.getType()) {
			case Basic_Integer:
			{
				if (b.getType() != Basic_Integer) {
					throw std::runtime_error("Cannot modulo a list to an integer");
				}
				const SFBasicInteger *ia = a.IntegerClass();
				const SFBasicInteger *ib = b.IntegerClass();
				return new SFBasicInteger(*ia % *ib);
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFLiteral* operator<<(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.isExtended() && b.isExtended()) {
			const SFExtended *ea = a.ExtClass();
			const SFExtended *eb = b.ExtClass();
			return *ea << *eb;
		}
		switch (a.getType()) {
			case Basic_Integer:
			{
				if (b.getType() != Basic_Integer) {
					throw std::runtime_error("Cannot shift left a list to an integer");
				}
				const SFBasicInteger *ia = a.IntegerClass();
				const SFBasicInteger *ib = b.IntegerClass();
				return new SFBasicInteger(*ia << *ib);
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	SFLiteral* operator>>(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.isExtended() && b.isExtended()) {
			const SFExtended *ea = a.ExtClass();
			const SFExtended *eb = b.ExtClass();
			return *ea >> *eb;
		}
		switch (a.getType()) {
			case Basic_Integer:
			{
				if (b.getType() != Basic_Integer) {
					throw std::runtime_error("Cannot shift left a list to an integer");
				}
				const SFBasicInteger *ia = a.IntegerClass();
				const SFBasicInteger *ib = b.IntegerClass();
				return new SFBasicInteger(*ia >> *ib);
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
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
}
