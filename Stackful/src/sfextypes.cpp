#include <exception>
#include <sstream>
#include <string>

#include "../include/sfatoms.hpp"
#include "../include/sfextypes.hpp"

SFLiteral_p tolist(const std::string &str) {
	SFBasicList *l = new SFBasicList();
	std::string::const_iterator it = str.begin();
	for (; it != str.end(); it++) {
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

SFFunctionCall::SFFunctionCall(const std::string &fn) : SFExtended(FunctionCall)
{
	this->push_back(getAtom(fn));
	this->push_back(SFBasicList());
}

SFFunctionCall::SFFunctionCall(const std::string &fn, SFLiteral_p p1) : SFExtended(FunctionCall)
{
	this->push_back(getAtom(fn));
	SFBasicList *params = new SFBasicList();
	params->push_back(p1);
	this->push_back(SFLiteral_p(params));
}

SFFunctionCall::SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2) : SFExtended(FunctionCall)
{
	this->push_back(getAtom(fn));
	SFBasicList *params = new SFBasicList();
	params->push_back(p1);
	params->push_back(p2);
	this->push_back(SFLiteral_p(params));
}

SFFunctionCall::SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2, SFLiteral_p p3) : SFExtended(FunctionCall)
{
	this->push_back(getAtom(fn));
	SFBasicList *params = new SFBasicList();
	params->push_back(p1);
	params->push_back(p2);
	params->push_back(p3);
	this->push_back(SFLiteral_p(params));
}

SFFunctionCall::SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2, SFLiteral_p p3, SFLiteral_p p4) : SFExtended(FunctionCall)
{
	this->push_back(getAtom(fn));
	SFBasicList *params = new SFBasicList();
	params->push_back(p1);
	params->push_back(p2);
	params->push_back(p3);
	params->push_back(p4);
	this->push_back(SFLiteral_p(params));
}

std::string SFFunctionCall::_str() const {
	std::stringstream ss;
	ss << "{FunctionCall: ";
	ss << getAtom(this->getFunction()->IntegerClass()->getValue());
	ss << this->getArguments()->str();
	ss << "}";
	return ss.str();
}

std::string SFOpChain::_str() const {
	std::stringstream ss;
	bool first = true;
	ss << "{OpChain: [";
	SFOpChain::iterator it = this->begin();
	for (; it != this->end(); it++) {
		SFLiteral_p item = *it;
		if (first)
			first = false;
		else
			ss << ", ";
		ss << item.get()->str();
	}
	ss << "]}";
	return ss.str();
}
