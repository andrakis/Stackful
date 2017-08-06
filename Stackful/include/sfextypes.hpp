#pragma once

// Extended types

#include "sfsharedtypes.hpp"
#include "sfatoms.hpp"

namespace stackful {
	enum ExtendedType {
		Atom,
		Integer,
		Float,
		String,
		List,
		FunctionCall,
		OpChain,
		Closure,
		FunctionDefinition,
		FunctionDefinitionNative
	};

	SFOpChain *toOpChain(SFLiteral_p opchain_p);
	SFClosure *toClosure(SFLiteral_p closure_p);

	extern SFLiteral_p atomNil, atomFalse, atomTrue, atomMissing;

	class SFAtom; class SFInteger; class SFFloat;
	class SFString; class SFList;

	class SFExtended : public SFBasicList {
	public:
		ExtendedType getExtendedType() const {
			return extendedType;
		}
		std::string str() const override {
			return _str();
		}
		std::string extStr() const {
			return _str();
		}
		virtual std::string extLiteral() const = 0;
		SFExtended(const SFExtended &copy) : SFBasicList(copy), extendedType(copy.getExtendedType()) {
		}
		typedef SFList_t::iterator iterator;
		bool isExtended() const {
			return true;
		}

	protected:
		SFExtended(ExtendedType extType) : SFBasicList(), extendedType(extType) {
		}
		SFExtended(ExtendedType extType, const SFBasicList &copy) : SFBasicList(copy), extendedType(extType) {
		}
		const ExtendedType extendedType;
		virtual std::string _str() const = 0;

		ExtendedType getPreferredType(ExtendedType a, ExtendedType b) {
			// Always convert to type a
			return a;
		}
	};

	class SFAtom : public SFExtended {
	public:
		SFAtom(SFInteger_t id) : SFExtended(Atom) {
			push_back(Atom);
			push_back(id);
		}
		SFAtom(const std::string &name) : SFExtended(Atom) {
			push_back(Atom);
			push_back(getAtom(name));
		}
		std::string extLiteral() const {
			return "'" + getAtom(operator[](1)->IntegerClass()->getValue()) + "'";
		}
	protected:
		std::string _str() const {
			std::string r = getAtom(operator[](1)->IntegerClass()->getValue());
			return r;
		}
	};

	class SFInteger : public SFExtended {
	public:
		SFInteger(const SFInteger_t val) : SFExtended(Integer) {
			push_back(Integer);
			push_back(val);
		}
		std::string extLiteral() const {
			return str();
		}
		SFInteger_t getRawValue() const {
			return operator[](1)->IntegerClass()->getValue();
		}
		operator SFFloat*() const;
		operator SFString*() const;
	protected:
		std::string _str() const {
			return operator[](1)->str();
		}
	};

	class SFFloat : public SFExtended {
	public:
		union doublebits {
			double valueDouble;
			SFInteger_t valueInteger[sizeof(double) / sizeof(SFInteger_t)];
		};
		SFFloat(double value) : SFExtended(Float) {
			push_back(Float);
			doublebits b;
			b.valueDouble = value;
			SFBasicList *l = new SFBasicList();
			for (unsigned int i = 0; i < sizeof(double) / sizeof(SFInteger_t); i++) {
				l->push_back(b.valueInteger[i]);
			}
			push_back(l);
		}
		std::string extLiteral() const {
			return str();
		}
		double getRawValue() const {
			return bitsToDouble();
		}
		operator SFInteger*() const;
		operator SFString*() const;
	protected:
		double bitsToDouble() const {
			doublebits b = { 0 };
			const SFBasicList &v = operator[](1)->ListClass();
			for (size_t i = 0; i < v.size(); i++) {
				b.valueInteger[i] = v[i]->IntegerClass()->getValue();
			}
			return b.valueDouble;
		}
		std::string _str() const {
			return std::to_string(bitsToDouble());
		}
	};

	class SFString : public SFExtended {
	public:
		SFString(const std::string &str) : SFExtended(String) {
			SFBasicList *l = new SFBasicList();
			std::string::const_iterator it = str.begin();
			for (; it != str.end(); ++it) {
				// Generally a char
				auto curr = *it;
				l->push_back(curr);
			}
			push_back(String);
			push_back(l);
		}
		std::string extLiteral() const {
			return "\"" + str() + "\"";
		}
		std::string getRawValue() const {
			return _str();
		}
	protected:
		std::string _str() const {
			std::stringstream s;

			const SFBasicList &v = operator[](1)->ListClass();
			for (size_t i = 0; i < v.size(); i++) {
				char c = (char)v[i]->IntegerClass()->getValue();
				s << c;
			}
			return s.str();
		}
	};

	class SFList : public SFExtended {
	public:
		SFList() : SFExtended(List) {
		}
		SFList(const SFList &copy) : SFExtended(List, copy) {
		}
		std::string extLiteral() const {
			return _literal(true);
		}
	protected:
		SFList(ExtendedType type) : SFExtended(type) { }
		SFList(ExtendedType type, const SFList &copy) : SFExtended(type, copy) {
		}
		std::string _str() const {
			return _literal(false);
		}

		std::string _literal(bool literal) const {
			std::stringstream s;
			s << "[";
			bool first = true;
			auto it = begin();
			for (; it != end(); ++it) {
				SFLiteral_p item = *it;
				if (first)
					first = false;
				else
					s << ", ";
				if (item->isExtended() && literal)
					s << item->ExtClass()->extLiteral();
				else
					s << item->str();
			}
			s << "]";
			return s.str();
		}
	};

	// A closure, which is a list of sublists, which make up a key/value store.
	// Each item in the list is composed of:
	//		[key, value]
	//	Where key is a list of integers representing a string.
	//	And value is an SFLiteral_p
	class SFClosure : public SFExtended {
	public:
		// Empty constructor with no parent
		SFClosure() : SFExtended(Closure), parent(nullptr), topmost(nullptr) {
		}
		// Copy constructor
		SFClosure(const SFClosure &copy) : SFExtended(Closure, copy), parent(copy.getParent()), topmost(copy.getTopmost()) {
		}
		// Construct with ops and no parent
		SFClosure(const SFBasicList &ops) : SFExtended(Closure, ops), parent(nullptr) {
		}
		// Construct with no ops and given parent (use shared ptr)
		SFClosure(SFLiteral_p _parent) : SFExtended(Closure), parent(_parent) {
		}
		// Construct with ops and given parent (use shared ptr)
		SFClosure(SFLiteral_p _parent, const SFBasicList &ops) : SFExtended(Closure, ops), parent(_parent) {
		}
		~SFClosure() {
		}
		SFLiteral_p getParent() const { return parent; }
		SFClosure *getParentObject() const {
			if (!hasParent())
				return nullptr;
			return static_cast<SFClosure*>(parent.get());
		}
		bool hasParent() const { return parent.get() != nullptr; }
		SFLiteral_p getTopmost() const {
			return topmost;
		}
		void setTopmost(SFLiteral_p _topmost) {
			this->topmost = _topmost;
		}
		SFClosure *getTopmostObject() const {
			return toClosure(this->topmost);
		}
		void setParent(SFLiteral_p _parent) {
			this->parent = _parent;
			this->topmost = toClosure(_parent)->getTopmost();
		}
		SFLiteral_p get(SFLiteral_p key) const throw(std::runtime_error) {
			SFList_t::iterator find = getByKey(key);
			if (find != end()) {
				const SFBasicList &list = find->get()->ListClass();
				return list[1];
			}
			if (hasParent())
				return getParentObject()->get(key);
			throw std::runtime_error("Key not found: " + key->str());
		}
		SFLiteral_p getOrMissing(SFLiteral_p key) const {
			SFList_t::iterator find = getByKey(key);
			if (find != end()) {
				const SFBasicList &list = find->get()->ListClass();
				return list[1];
			}
			if (hasParent())
				return getParentObject()->getOrMissing(key);
			return atomMissing;
		}
		void set(SFLiteral_p _key, SFLiteral_p _value) {
			if (trySet(_key, _value))
				return;
			setImmediate(_key, _value);
		}
		void setImmediate(SFLiteral_p key, SFLiteral_p _value) {
			SFList_t::iterator find = getByKey(key);
			SFBasicList *newKeyValue = new SFBasicList();
			newKeyValue->push_back(key);
			newKeyValue->push_back(_value);
			SFLiteral_p newKeyValue_p(newKeyValue);
			if (find == end()) {
				this->push_back(newKeyValue_p);
			} else {
				find->swap(newKeyValue_p);
			}
		}
		std::string extLiteral() const {
			return str();
		}
	protected:
		SFLiteral_p parent;
		SFLiteral_p topmost;
		SFList_t::iterator getByKey(SFLiteral_p key) const {
			SFList_t::iterator find = this->value->begin();
			for (; find != end(); ++find) {
				// [key, value]
				const SFBasicList *list = find->get()->ListClass();
				// check key
				if (list->at(0)->equals(*key)) {
					break;
				}
			}
			return find;
		}
		bool hasKey(SFLiteral_p key) const {
			return getByKey(key) != end();
		}
		bool trySet(SFLiteral_p key, SFLiteral_p _value) {
			if (hasKey(key)) {
				setImmediate(key, _value);
				return true;
			}
			if (hasParent() && toClosure(parent)->trySet(key, _value))
				return true;
			return false;
		}
		std::string _str() const {
			SFClosure::iterator it = begin();
			std::stringstream ss;
			ss << "{Closure:";
			for (; it != end(); ++it) {
				SFLiteral_p p(*it);
				ss << p->str();
			}
			if (hasParent())
				ss << "{Parent:" + getParentObject()->str() + "}";
			ss << "}";
			return ss.str();
		}
	};

	// An OpChain, containing a Closure and operations to run.
	class SFOpChain : public SFExtended {
	public:
		// An empty OpChain
		SFOpChain()
		: SFExtended(OpChain), parent(nullptr), closure(new SFClosure()), immediate(false),
		functionEntry("") {
			this->getClosureObject()->setTopmost(this->getClosurePtr());
		}
		// Copy constructor
		SFOpChain(const SFOpChain &copy)
		: SFExtended(OpChain, copy), parent(copy.getParentPtr()), closure(copy.getClosurePtr()),
		immediate(false), functionEntry("") {
			if(this->getClosureObject()->getTopmost().get() == nullptr)
				this->getClosureObject()->setTopmost(this->getClosurePtr());
		}
		// Initialize with given parent
		SFOpChain(SFLiteral_p _parent) : SFExtended(OpChain), parent(_parent),
		closure(new SFClosure(toOpChain(_parent)->getClosurePtr())),
		immediate(false), functionEntry("") {
			if(this->getClosureObject()->getTopmost().get() == nullptr)
				this->getClosureObject()->setTopmost(this->getClosurePtr());
		}
		// Initialize with given parent and ops
		SFOpChain(SFLiteral_p _parent, const SFOpChain *ops)
		: SFExtended(OpChain, ops), parent(_parent),
		closure(new SFClosure(toOpChain(_parent)->getClosurePtr())),
		immediate(false), functionEntry("") {
			if(this->getClosureObject()->getTopmost().get() == nullptr)
				this->getClosureObject()->setTopmost(this->getClosurePtr());
		}
		SFLiteral_p getParentPtr() const { return parent; }
		SFLiteral_p getClosurePtr() const { return closure; }
		SFOpChain *getParentObject() const { return toOpChain(parent); };
		SFClosure *getClosureObject() const { return toClosure(closure); }
		std::string extLiteral() const {
			return str();
		}
		SFLiteral_p get() const {
			if (pos >= (int)size())
				return nullptr;
			return at(pos);
		}
		SFLiteral_p next() {
			pos++;
			return get();
		}
		void rewind() {
			pos = -1;
		}

		void importClosure(const SFBuiltinDefinitions_t &functions);
		bool getImmediate() const { return immediate; }
		void setImmediate(bool imm) { immediate = imm; }
		std::string getFunctionEntry() const { return functionEntry; }
		void setFunctionEntry(const std::string &entry) { functionEntry = entry; }
	protected:
		std::string _str() const;
		SFLiteral_p parent;
		SFLiteral_p closure;
		int pos = -1;
		bool immediate;
		std::string functionEntry;
	};

	SFBasicList sfvar(const std::string &str);
	SFBasicList sfvarfloat(const double value);
	SFBasicList sfvar(const SFInteger_t value);
	SFBasicList sfatom(const std::string &s);
	ExtendedType identifyLiteral(const SFBasicList &l);
	std::string varstr(const SFBasicList &l);

	class SFFunctionCall : public SFList {
	public:
		SFFunctionCall(const SFFunctionCall &copy) : SFList(FunctionCall, copy), details(copy.getDetails()) {
		}
		SFFunctionCall(const std::string &fn);
		SFFunctionCall(const std::string &fn, SFLiteral_p p1);
		SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2);
		SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2, SFLiteral_p p3);
		SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2, SFLiteral_p p3, SFLiteral_p p4);
		std::string extLiteral() const {
			return _literal(true);
		}
		SFLiteral_p getFunction() const {
			return at(0);
		}
		void setFunction(SFLiteral_p fn) {
			at(0).swap(fn);
		}
		SFList *getArguments() const {
			SFLiteral_p args = at(1);
			return static_cast<SFList*>(args.get());
		}
		SFFunctionArity_t getDetails() const { return details; }
	protected:
		std::string _str() const;
		SFFunctionArity_t details;
	};

	typedef std::map<SFInteger_t, SFLiteral_p> atomPtrsById_t;
	extern atomPtrsById_t atomPtrsById;
	SFLiteral_p getAtomPtr(SFInteger_t id);
	SFLiteral_p getAtomPtr(const std::string &name);

	SFExtended* operator+(const SFExtended &a, const SFExtended &b) throw(std::runtime_error);
	SFExtended* operator-(const SFExtended &a, const SFExtended &b) throw(std::runtime_error);
	SFExtended* operator*(const SFExtended &a, const SFExtended &b) throw(std::runtime_error);
	SFExtended* operator/(const SFExtended &a, const SFExtended &b) throw(std::runtime_error);
	SFExtended* operator|(const SFExtended &a, const SFExtended &b) throw(std::runtime_error);
	SFExtended* operator^(const SFExtended &a, const SFExtended &b) throw(std::runtime_error);
	SFExtended* operator%(const SFExtended &a, const SFExtended &b) throw(std::runtime_error);
	SFExtended* operator<<(const SFExtended &a, const SFExtended &b) throw(std::runtime_error);
	SFExtended* operator>>(const SFExtended &a, const SFExtended &b) throw(std::runtime_error);
	bool operator<(const SFExtended &a, const SFExtended &b) throw(std::runtime_error);
	bool operator>(const SFExtended &a, const SFExtended &b) throw(std::runtime_error);
	bool operator<=(const SFExtended &a, const SFExtended &b) throw(std::runtime_error);
	bool operator>=(const SFExtended &a, const SFExtended &b) throw(std::runtime_error);
}
