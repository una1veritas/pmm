//============================================================================
// Name        : XBWTrie.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstddef>
#include <vector>

#include "ACMachine_varray.h"

using namespace std;

struct XBWTNode {
	uint16_t islast : 1;
	uint16_t isleaf : 1;
	uint16_t alph : 8;
};

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	cout << sizeof(XBWTNode) << endl;

	ACMachine pmm;
	vector<string> words = {
			"cat",
			"cap",
			"cab",
			"at",
			"attention",
			"abandon",
			"category",
			"apple",
			"cape",
			"bat",
	};

	pmm.addPatterns(words);

	cout << pmm << endl;

	return 0;
}
