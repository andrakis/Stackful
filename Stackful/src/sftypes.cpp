#include "../stdafx.h"
#include <iostream>
#include <sstream>
#include "../include/sftypes.hpp"
#include "../include/sfextypes.hpp"

namespace stackful {
	SFLiteral::~SFLiteral() {
#ifdef PETTY_DEBUG
		debug << "A ";
		if (this->type == Basic_Integer) debug << "number " << this->IntegerClass()->getValue();
		else if (this->type == Basic_List) debug << "list";
		debug << " has died" << std::endl;
#endif
#ifdef _LIFETIME
		auto now = std::chrono::system_clock::now();
		std::stringstream ss;
		ss << "A ";
		if (this->type == Basic_Integer) {
			ss << "basic number ";
			ss << this->IntegerClass()->getValue();
		} else if (this->type == Basic_List) {
			ss << "basic list";
		}
		ss << " has died (lifetime: ";
		ss << std::chrono::duration_cast<std::chrono::milliseconds>(now - this->start).count();
		ss << "ms)";
		std::cerr << ss.str() << std::endl;
#endif
	}

	SFBasicInteger* SFLiteral::IntegerClass() {
		if (this->type != Basic_Integer)
			throw std::runtime_error("Not a number");
		return static_cast<SFBasicInteger*>(this);
	}
	const SFBasicInteger* SFLiteral::IntegerClass() const {
		if (this->type != Basic_Integer)
			throw std::runtime_error("Not a number");
		return static_cast<const SFBasicInteger*>(this);
	}
	SFBasicList* SFLiteral::ListClass() {
		if (this->type != Basic_List)
			throw std::runtime_error("Not a list");
		return static_cast<SFBasicList*>(this);
	}
	const SFBasicList* SFLiteral::ListClass() const {
		if (this->type != Basic_List)
			throw std::runtime_error("Not a list");
		return static_cast<const SFBasicList*>(this);
	}

	SFBasicInteger operator + (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() + b.getValue());
	}
	SFBasicInteger operator - (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() - b.getValue());
	}
	SFBasicInteger operator * (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() * b.getValue());
	}
	SFBasicInteger operator / (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() / b.getValue());
	}
	SFBasicInteger operator | (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() | b.getValue());
	}
	SFBasicInteger operator & (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() & b.getValue());
	}
	SFBasicInteger operator ^ (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() ^ b.getValue());
	}
	SFBasicInteger operator % (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() % b.getValue());
	}
	SFBasicInteger operator ! (const SFBasicInteger &a) {
		return SFBasicInteger(!a.getValue());
	}
	SFBasicInteger operator << (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() << b.getValue());
	}
	SFBasicInteger operator >> (const SFBasicInteger &a, const SFBasicInteger &b) {
		return SFBasicInteger(a.getValue() >> b.getValue());
	}
	bool operator < (const SFBasicInteger &a, const SFBasicInteger &b) {
		return a.getValue() < b.getValue();
	}
	bool operator > (const SFBasicInteger &a, const SFBasicInteger &b) {
		return a.getValue() > b.getValue();
	}
	bool operator <= (const SFBasicInteger &a, const SFBasicInteger &b) {
		return a.getValue() <= b.getValue();
	}
	bool operator >= (const SFBasicInteger &a, const SFBasicInteger &b) {
		return a.getValue() >= b.getValue();
	}

	SFBasicList operator + (const SFBasicList &a, const SFBasicList &b) {
		SFBasicList result(a);
		result.ShallowCopy(&b);
		return result;
	}

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

	bool operator<(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.isExtended() && b.isExtended()) {
			const SFExtended *ea = a.ExtClass();
			const SFExtended *eb = b.ExtClass();
			return *ea < *eb;
		}
		switch (a.getType()) {
			case Basic_Integer:
			{
				if (b.getType() != Basic_Integer) {
					throw std::runtime_error("Cannot compare a list to an integer");
				}
				const SFBasicInteger *ia = a.IntegerClass();
				const SFBasicInteger *ib = b.IntegerClass();
				return *ia < *ib;
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	bool operator>(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.isExtended() && b.isExtended()) {
			const SFExtended *ea = a.ExtClass();
			const SFExtended *eb = b.ExtClass();
			return *ea > *eb;
		}
		switch (a.getType()) {
			case Basic_Integer:
			{
				if (b.getType() != Basic_Integer) {
					throw std::runtime_error("Cannot compare a list to an integer");
				}
				const SFBasicInteger *ia = a.IntegerClass();
				const SFBasicInteger *ib = b.IntegerClass();
				return *ia > *ib;
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	bool operator<=(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.isExtended() && b.isExtended()) {
			const SFExtended *ea = a.ExtClass();
			const SFExtended *eb = b.ExtClass();
			return *ea <= *eb;
		}
		switch (a.getType()) {
			case Basic_Integer:
			{
				if (b.getType() != Basic_Integer) {
					throw std::runtime_error("Cannot compare a list to an integer");
				}
				const SFBasicInteger *ia = a.IntegerClass();
				const SFBasicInteger *ib = b.IntegerClass();
				return *ia <= *ib;
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
	bool operator>=(const SFLiteral &a, const SFLiteral &b) throw(std::runtime_error) {
		if (a.isExtended() && b.isExtended()) {
			const SFExtended *ea = a.ExtClass();
			const SFExtended *eb = b.ExtClass();
			return *ea >= *eb;
		}
		switch (a.getType()) {
			case Basic_Integer:
			{
				if (b.getType() != Basic_Integer) {
					throw std::runtime_error("Cannot compare a list to an integer");
				}
				const SFBasicInteger *ia = a.IntegerClass();
				const SFBasicInteger *ib = b.IntegerClass();
				return *ia >= *ib;
			}
			default:
				throw std::runtime_error("Invalid operation");
		}
	}
}
