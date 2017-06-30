// Stackful.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include "include/sftypes.hpp"

class SFState {
public:
	SFState() {
		atomMap.push_back("nil");
		atomMap.push_back("false");
		atomMap.push_back("true");
	}
	virtual ~SFState() {
	}
protected:
	SFList ops;
	SFAtomMap_t atomMap;
};

int main()
{
	SFInteger i1(1);
	SFInteger i2(5);
	SFInteger i3(i1 ^ i2);
	SFList l1;

	l1.push_back(i1);
	l1.push_back(i2);
	l1.push_back(i3);

	SFList l2(l1);
	SFList l3(l1 + l2);
	SFList l4(l1);

	l4.push_back(l1);

	SFState state;

	std::cout << l4.str() << std::endl;
    return 0;
}

