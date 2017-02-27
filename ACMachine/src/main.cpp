//============================================================================
// Name        : ACMachine.cpp
// Author      : Sin Shimozono
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include "ACMachine.h"

int main(const int argc, const char * argv[]) {
	cout << "Hello World!!!" << endl; // prints Hello World!!!
	cout << "arguments " << argc << ", values are " << endl;
	for(int i = 1; i < argc; i++)
		cout << "'" << argv[i] << "', ";
	cout << endl;


	ACMachine pmm("why");
	for ( auto it : pmm.search("The reason why you were there.") ) {
		cout << *it << ", ";
	}
	cout << endl;

	cout << "bye." << endl << endl;
	return 0;
}
