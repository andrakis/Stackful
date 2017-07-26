#pragma once

// Extended types

#include "sftypes.hpp"
#include "sfatoms.hpp"

namespace stackful {

	class SFOpChain;
	class SFClosure;

	typedef std::shared_ptr<SFOpChain> SFOpChain_p;
	typedef std::shared_ptr<SFClosure> SFClosure_p;

	enum ExtendedType {
		Atom,
		Integer,
		Float,
		String,
		List,
		FunctionCall,
		OpChain,
		Closure
	};

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
	};

	class SFAtom : public SFExtended {
	public:
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
			for (auto i = 0; i < sizeof(double) / sizeof(SFInteger_t); i++) {
				l->push_back(b.valueInteger[i]);
			}
			push_back(l);
		}
		std::string extLiteral() const {
			return str();
		}
	protected:
		std::string _str() const {
			doublebits b = { 0 };
			const SFBasicList &v = operator[](1)->ListClass();
			for (size_t i = 0; i < v.size(); i++) {
				b.valueInteger[i] = v[i]->IntegerClass()->getValue();
			}
			return std::to_string(b.valueDouble);
		}
	};

	class SFString : public SFExtended {
	public:
		SFString(const std::string &str) : SFExtended(String) {
			SFBasicList *l = new SFBasicList();
			std::string::const_iterator it = str.begin();
			for (; it != str.end(); it++) {
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
			for (; it != end(); it++) {
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
		SFClosure(const SFBasicList &ops) : SFExtended(Closure, ops), parent(nullptr), topmost(nullptr) {
		}
		// Construct with no ops and given parent (use shared ptr)
		SFClosure(const SFClosure_p &parent) : SFExtended(Closure), parent(parent), topmost(parent->getTopmost()) {
		}
		// Construct with ops and given parent (use shared ptr)
		SFClosure(const SFClosure_p &parent, const SFBasicList &ops) : SFExtended(Closure, ops), parent(parent), topmost(parent->getTopmost()) {
		}
		~SFClosure() {
		}
		SFClosure_p getParent() const { return parent; }
		SFClosure_p getTopmost() const {
			return topmost;
		}
		void setParent(const SFClosure_p &parent) {
			this->parent = parent;
			this->topmost = parent->getTopmost();
		}
		SFLiteral_p get(const SFList_p &key) const throw(std::runtime_error) {
			return get(key.get());
		}
		SFLiteral_p get(const SFBasicList &key) const throw(std::runtime_error) {
			SFList_t::iterator find = getByKey(key);
			if (find == end())
				throw std::runtime_error("Key not found: " + key.str());
			const SFBasicList &list = find->get()->ListClass();
			return list[1];
		}
		void set(const SFBasicList &key, const SFLiteral_p &value) {
			if (trySet(key, value))
				return;
			setImmediate(key, value);
		}
		void setImmediate(const SFBasicList &key, const SFLiteral_p &value) {
			SFList_t::iterator find = getByKey(key);
			SFBasicList *newKeyValue = new SFBasicList();
			newKeyValue->push_back(key);
			newKeyValue->push_back(value);
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
		SFClosure_p parent;
		SFClosure_p topmost;
		SFList_t::iterator getByKey(const SFList_p &key) const {
			return getByKey(key.get());
		}

		SFList_t::iterator getByKey(const SFBasicList &key) const {
			SFList_t::iterator find = this->value->begin();
			for (; find != end(); find++) {
				// [key, value]
				const SFBasicList &list = find->get()->ListClass();
				// check key
				if (list[0]->ListClass()->isEqual(&key))
					break;
			}
			return find;
		}
		bool hasKey(const SFBasicList &key) const {
			return getByKey(key) != end();
		}
		bool trySet(const SFBasicList &key, const SFLiteral_p &value) {
			if (hasKey(key)) {
				setImmediate(key, value);
				return true;
			}
			if (parent.get() != nullptr && parent->trySet(key, value))
				return true;
			return false;
		}
	protected:
		std::string _str() const {
			return "CLOSURE";
		}
	};

	// An OpChain, containing a Closure and operations to run.
	class SFOpChain : public SFExtended {
	public:
		// An empty OpChain
		SFOpChain() : SFExtended(Closure), parent(nullptr), closure(new SFClosure()) {
		}
		// Copy constructor
		SFOpChain(const SFOpChain &copy) : SFExtended(Closure, copy), parent(copy.getParent()), closure(copy.getClosure()) {
		}
		// Initialize with given parent
		SFOpChain(const SFOpChain_p &parent) : SFExtended(Closure), parent(parent), closure(new SFClosure()) {
		}
		// Initialize with given parent and ops
		SFOpChain(const SFOpChain_p &parent, const SFOpChain *ops) : SFExtended(OpChain, ops), parent(parent), closure(parent->getClosure()) {
		}
		SFOpChain_p getParent() const { return parent; };
		SFClosure_p getClosure() const { return closure; }
		std::string extLiteral() const {
			return str();
		}
		SFExtended *get() const {
			if (pos >= size())
				return nullptr;
			SFLiteral_p p(at(pos));
			return dynamic_cast<SFExtended*>(p.get());
		}
		SFExtended *next() {
			pos++;
			return get();
		}
		void rewind() {
			pos = -1;
		}
	protected:
		std::string _str() const;
		SFOpChain_p parent;
		SFClosure_p closure;
		size_t pos = -1;
	};

	SFBasicList sfvar(const std::string &str);
	SFBasicList sfvarfloat(const double value);
	SFBasicList sfvar(const SFInteger_t value);
	SFBasicList sfatom(const std::string &s);
	ExtendedType identifyLiteral(const SFBasicList &l);
	std::string varstr(const SFBasicList &l);

	class SFFunctionCall : public SFList {
	public:
		SFFunctionCall(const SFFunctionCall &copy) : SFList(FunctionCall, copy) {
		}
		SFFunctionCall(const std::string &fn);
		SFFunctionCall(const std::string &fn, SFLiteral_p p1);
		SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2);
		SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2, SFLiteral_p p3);
		SFFunctionCall(const std::string &fn, SFLiteral_p p1, SFLiteral_p p2, SFLiteral_p p3, SFLiteral_p p4);
		std::string extLiteral() const {
			return _literal(true);
		}
		SFExtended *getFunction() const {
			return at(0)->ExtClass();
		}
		SFExtended *getArguments() const {
			return at(1)->ExtClass();
		}
	protected:
		std::string _str() const;
	};
}
