#pragma once

#include "../stdafx.h"
#include <exception>
#include <inttypes.h>	// uint32_t
#include <iostream>
#include <memory>		// shared_ptr
#include <sstream>		// stringstream
#include <vector>

#include "sfdebug.hpp"

namespace stackful {

	// The size of our basic number type
	typedef uint32_t SFInteger_t;

	// Forward declarations
	class SFLiteral;
	class SFBasicInteger;
	class SFBasicList;

	// Extended forward declarations
	class SFExtended;

	enum SFType {
		Basic_Integer,
		Basic_List
	};

	// The generic literal value
	class SFLiteral {
	public:
		virtual ~SFLiteral();
		SFType getType() const { return type; }

		SFBasicInteger *IntegerClass();
		SFBasicList *ListClass();
		SFExtended *ExtClass();
		const SFBasicInteger *IntegerClass() const;
		const SFBasicList *ListClass() const;
		const SFExtended *ExtClass() const;

		virtual std::string str() const = 0;

		bool operator == (const SFLiteral &other) const {
			return equals(other);
		}
		bool operator != (const SFLiteral &other) const {
			return !equals(other);
		}

		bool equals(const SFLiteral &other) const {
			if (this->getType() != other.getType())
				return false;
			return _isequal(other);
		}

		virtual bool isExtended() const {
			return false;
		}

	protected:
		SFLiteral(const SFType t) : type(t) {
		}

		virtual bool _isequal(const SFLiteral &other) const = 0;

		const SFType type;
	};

	// The specialised value
	template<class T>
	class SFLiteralT : public SFLiteral {
	public:
		T getValue() const { return value; }
	protected:
		SFLiteralT(const T v, SFType type) : SFLiteral(type), value(v) {
		}
		bool _isequal(const SFLiteral &other) const {
			return literalEquals(other);
		}
		virtual bool literalEquals(const SFLiteral &other) const = 0;
		T value;
	};

	// Generic number class
	class SFBasicInteger : public SFLiteralT<const SFInteger_t> {
	public:
		SFBasicInteger(const SFInteger_t v) : SFLiteralT(v, Basic_Integer) {
		}
		SFBasicInteger(const SFBasicInteger &v) : SFLiteralT(v.getValue(), Basic_Integer) {
		}
		std::string str() const {
			std::stringstream ss;
			ss << value;
			return ss.str();
		}
	protected:
		bool literalEquals(const SFLiteral &other) const {
			return value == other.IntegerClass()->getValue();
		}
	};

	SFBasicInteger operator + (const SFBasicInteger &a, const SFBasicInteger &b);
	SFBasicInteger operator - (const SFBasicInteger &a, const SFBasicInteger &b);
	SFBasicInteger operator * (const SFBasicInteger &a, const SFBasicInteger &b);
	SFBasicInteger operator / (const SFBasicInteger &a, const SFBasicInteger &b);
	SFBasicInteger operator | (const SFBasicInteger &a, const SFBasicInteger &b);
	SFBasicInteger operator & (const SFBasicInteger &a, const SFBasicInteger &b);
	SFBasicInteger operator ^ (const SFBasicInteger &a, const SFBasicInteger &b);
	SFBasicInteger operator % (const SFBasicInteger &a, const SFBasicInteger &b);
	SFBasicInteger operator ! (const SFBasicInteger &a);
	SFBasicInteger operator << (const SFBasicInteger &a, const SFBasicInteger &b);
	SFBasicInteger operator >> (const SFBasicInteger &a, const SFBasicInteger &b);

	typedef std::shared_ptr<SFLiteral> SFLiteral_p;
	typedef std::vector<SFLiteral_p> SFList_t;

	// A list that may contain numbers or other lists
	class SFBasicList : public SFLiteralT<SFList_t*> {
	public:
		typedef SFList_t::iterator iterator;
		SFBasicList() : SFLiteralT(new SFList_t(), Basic_List) {
		}
		SFBasicList(const SFBasicList &l) : SFLiteralT(new SFList_t(), Basic_List) {
			ShallowCopy(l);
		}
		SFBasicList(const SFBasicList *l) : SFLiteralT(new SFList_t(), Basic_List) {
			ShallowCopy(l);
		}
		SFBasicList &operator=(SFBasicList &list) {
			std::swap(value, list.value);
			return (*this);
		}
		void ShallowCopy(const SFBasicList *l) {
			SFList_t::iterator it = l->begin();
			for (; it != l->end(); ++it) {
				value->push_back(*it);
			}
		}
		void ShallowCopy(const SFBasicList &l) {
			// Pass to above function
			ShallowCopy(&l);
		}
		~SFBasicList() {
			clear();
		}
		void clear() {
			value->clear();
		}
		SFLiteral_p pop_back() throw(std::runtime_error) {
			if (size() == 0)
				throw std::runtime_error("pop_back(): Basic_List is empty");
			SFLiteral_p p = value->at(size() - 1);
			value->pop_back();
			return p;
		}
		void push_back(SFLiteral_p p) {
			value->push_back(p);
		}
		void push_back(SFLiteral *v) {
			value->push_back(SFLiteral_p(v));
		}
		void push_back(const SFBasicInteger &i) {
			push_back(new SFBasicInteger(i));
		}
		void push_back(const SFBasicList &l) {
			SFBasicList *n = new SFBasicList(l);
			push_back(n);
			// n will be freed by the shared pointer
		}
		std::string str() const {
			std::stringstream ss;
			SFList_t::iterator it = begin();
			bool first = true;
			ss << "[";
			for (; it != end(); ++it) {
				if (first)
					first = false;
				else
					ss << ", ";
				ss << it->get()->str();
			}
			ss << "]";
			return ss.str();
		}
		size_t size() const {
			return value->size();
		}
		SFList_t::iterator begin() const { return value->begin(); }
		SFList_t::iterator end() const { return value->end(); }
		SFLiteral_p operator[](SFList_t::size_type _Pos) const {
			return at(_Pos);
		}
		SFLiteral_p at(const SFList_t::size_type _Pos) const {
			return value->at(_Pos);
		}
		void set(const SFList_t::size_type _Pos, const SFLiteral_p &v) {
			value->at(_Pos) = v;
		}
		bool isEqual(const SFBasicList *other_list) const {
			if (size() != other_list->size())
				return false;
			SFList_t::iterator our_it = begin();
			SFList_t::iterator other_it = other_list->begin();
			for (; our_it != end(), other_it != other_list->end(); ++our_it, ++other_it) { //-V521
				const SFLiteral *our_item = our_it->get();
				const SFLiteral *other_item = other_it->get();
				if (our_item->equals(*other_item) == false)
					return false;
			}
			return true;
		}
	protected:
		// Compare every item in the list. May result in recursive calls.
		bool literalEquals(const SFLiteral &other) const {
			const SFBasicList *other_list = other.ListClass();
			return isEqual(other_list);
		}
	};

	typedef std::shared_ptr<SFBasicList> SFList_p;

	SFBasicList operator + (const SFBasicList &a, const SFBasicList &b);

	SFLiteral* operator+(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error);
	SFLiteral* operator-(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error);
	SFLiteral* operator*(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error);
	SFLiteral* operator/(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error);
	SFLiteral* operator|(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error);
	SFLiteral* operator^(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error);
	SFLiteral* operator%(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error);
	SFLiteral* operator<<(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error);
	SFLiteral* operator>>(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error);

	bool operator<(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error);
	bool operator>(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error);
	bool operator<=(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error);
	bool operator>=(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error);
}
