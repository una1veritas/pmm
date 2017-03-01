//============================================================================
// Name        : ACMachine.cpp
// Author      : Sin Shimozono
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>

using namespace std;

#include "ACMachine.h"

int main(const int argc, const char * argv[]) {
	cout << "Hello World!!!" << endl; // prints Hello World!!!
	cout << "arguments " << argc << ", values are " << endl;
	if ( argc == 1 ) {
		cout << "none." << endl;
	} else {
		for(int i = 1; i < argc; i++)
			cout << "'" << argv[i] << "', ";
		cout << endl;
	}

	acm pmm;

	pmm.resetState();
	pmm.addPath(string("papaya"));
	pmm.addOutput("papaya");
	pmm.addFailures();


	cout << pmm << endl << endl;
	for ( auto occurr : pmm.search("pappapararirapapayapa") ) {
		cout << occurr.second << "@" << occurr.first << "," << endl;
	}
	cout << endl;

	cout << "bye." << endl << endl;
	return 0;
}
