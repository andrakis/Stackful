#pragma once

#include "sftypes.hpp"
#include "sfextypes.hpp"

template<class T>
class JSIterator {
public:
	JSIterator(stackful::SFLiteral_p _ptr) : index(-1), ptr(_ptr) {
		refresh();
	}
	T getPtrObject() const {
		return static_cast<T>(ptr.get());
	}
	void refresh() {
		length = getPtrObject()->size();
	}
	void rewind() {
		index = 0;
	}
	stackful::SFLiteral_p get(const int &_index) const {
		return getPtrObject()->at(_index);
	}
	stackful::SFLiteral_p get() const {
		return get(index);
	}
	void set(int _index, stackful::SFLiteral_p val) const {
		getPtrObject()->at(_index) = val;
	}
	void set_current(stackful::SFLiteral_p val) const {
		set(index, val);
	}
	stackful::SFLiteral_p next() {
		refresh();
		if (index >= length)
			return stackful::atomNil;
		return get(index++);
	}
	stackful::SFLiteral_p prev() {
		if (index == 1)
			throw std::runtime_error("Cannot move before first element");
		--index;
		return get();
	}
protected:
	int index;
	stackful::SFLiteral_p ptr;
	size_t length;
};
