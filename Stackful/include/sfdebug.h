#pragma once

#include <iostream>
#include <string>

namespace stackful {
	class SFDebug
	{
	public:
		SFDebug() : enabled(false) { }
		~SFDebug() { }

		// Generic << operator
		template<typename T>
		SFDebug& operator<< (const T &v) {
			if (enabled)
				getDestStream() << v;
			return (*this);
		}

		// Wrappers for various streams
		// Allows std::endl
		SFDebug& operator<<(std::ostream& (*f)(std::ostream &)) {
			if (enabled)
				f(getDestStream());
			return (*this);
		}

		void setEnabled(bool v) {
			enabled = v;
		}
		bool getEnabled() const {
			return enabled;
		}
	protected:
		// Get the destination stream
		std::ostream &getDestStream() const {
			return std::cerr;
		}

		bool enabled;
	};

	extern SFDebug debug, *_Ptr_debug;
}