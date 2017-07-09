#include <exception>
#include <string>

#include "../include/sfatoms.hpp"
#include "../include/sfextypes.hpp"

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
	return SFXString(str);
}

SFList sfvarfloat(const double value) {
	return SFXFloat(value);
}

SFList sfvar(const SFInteger_t value) {
	return sfvariable(Integer, SFLiteral_p(new SFInteger(value)));
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
	return l.str();
}

SFXFunctionCall::SFXFunctionCall(const std::string &fn) : SFExtended(FunctionCall)
{
	this->push_back(getAtom(fn));
	this->push_back(SFList());
}

SFXFunctionCall::SFXFunctionCall(const std::string &fn, SFLiteral_p p1) : SFExtended(FunctionCall)
{
	this->push_back(getAtom(fn));
	SFList *params = new SFList();
	params->push_back(p1);
	this->push_back(SFLiteral_p(params));
}

SFXFunctionCall::SFXFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2) : SFExtended(FunctionCall)
{
	this->push_back(getAtom(fn));
	SFList *params = new SFList();
	params->push_back(p1);
	params->push_back(p2);
	this->push_back(SFLiteral_p(params));
}

SFXFunctionCall::SFXFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2, SFLiteral_p p3) : SFExtended(FunctionCall)
{
	this->push_back(getAtom(fn));
	SFList *params = new SFList();
	params->push_back(p1);
	params->push_back(p2);
	params->push_back(p3);
	this->push_back(SFLiteral_p(params));
}

SFXFunctionCall::SFXFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2, SFLiteral_p p3, SFLiteral_p p4) : SFExtended(FunctionCall)
{
	this->push_back(getAtom(fn));
	SFList *params = new SFList();
	params->push_back(p1);
	params->push_back(p2);
	params->push_back(p3);
	params->push_back(p4);
	this->push_back(SFLiteral_p(params));
}
