#include <exception>
#include <string>

#include "../include/sfatoms.hpp"
#include "../include/sfextypes.hpp"

union doublebits {
	double valueDouble;
	SFInteger_t valueInteger[sizeof(double) / sizeof(SFInteger_t)];
};

SFLiteral_p tolist(const std::string &str) {
	SFList *l = new SFList();
	std::string::const_iterator it = str.begin();
	for (; it != str.end(); it++) {
		// Generally a char
		auto curr = *it;
		l->push_back(curr);
	}
	return SFLiteral_p(l);
}

SFList sfvariable(const SFInteger_t type, SFLiteral_p value) {
	SFList v;
	v.push_back(type);
	v.push_back(value);
	return v;
}

SFList sfvar(const std::string &str) {
	return sfvariable(String, tolist(str));
}

SFList sfvar(const double value) {
	doublebits b;
	b.valueDouble = value;
	SFList *l = new SFList();
	for (auto i = 0; i < sizeof(double) / sizeof(SFInteger_t); i++) {
		l->push_back(b.valueInteger[i]);
	}
	return sfvariable(Float, SFLiteral_p(l));
}

SFList sfvar(const SFInteger_t value) {
	return sfvariable(Number, SFLiteral_p(new SFInteger(value)));
}

SFList sfatom(const std::string &s) {
	return sfvariable(Atom, SFLiteral_p(new SFInteger(getAtom(s))));
}

ExtendedType identifyLiteral(const SFList &l) {
	if (l.size() != 2)
		throw std::runtime_error(std::string("Invalid literal: ") + l.str());
	SFInteger *firstElement = l[0]->IntegerClass();
	return (ExtendedType)firstElement->getValue();
}

std::string varstr(const SFList &l) {
	switch (identifyLiteral(l)) {
		case Atom:
			return getAtom(l[1].get()->IntegerClass()->getValue());
		case Number:
			return l[1].get()->str();
		case String:
		{
			std::stringstream s;
			const SFList &v = l[1].get()->ListClass();
			for (size_t i = 0; i < v.size(); i++) {
				char c = (char)v[i].get()->IntegerClass()->getValue();
				s << c;
			}
			return s.str();
		}
		case Float:
		{
			doublebits b;
			const SFList &v = l[1].get()->ListClass();
			for (size_t i = 0; i < v.size(); i++) {
				b.valueInteger[i] = v[i].get()->IntegerClass()->getValue();
			}
			return std::to_string(b.valueDouble);
		}
		case OpChain:
			// TODO: Nicer
			return l[1].get()->str();
		default:
			throw std::runtime_error("Unknown value type");
	}
}