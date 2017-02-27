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


	ACMachine pmm;
	pmm.addPath("keyword");
	pmm.addOutput("keyword");

	cout << "AC Machine: " << pmm << endl << endl;
	for ( auto occurrence : pmm.search("The reason why you were there.") ) {
		cout << occurrence.first << ", ";
	}
	cout << endl;

	cout << "bye." << endl << endl;
	return 0;
}
