//============================================================================
// Name        : ACMachine.cpp
// Author      : Sin Shimozono
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

#include "ACMachine.h"
#include "commandargs.h"

#include <time.h>

int main(const int argc, const char * argv[]) {
	ifstream ifs;
	int wordcount_max = 10;
	bool show_machine = false;
	cout << "Hello World!!!" << endl; // prints Hello World!!!
	commandargs args(argc, argv);

	pair<bool,const char*> opt;
	opt = args.getopt("-k");
	if ( opt.first ) {
		wordcount_max = atol(opt.second);
	}
	opt = args.getopt("-f");
	if ( opt.first ) {
		string fname = string(opt.second);
		ifs.open(fname);
	}
	opt = args.getopt("-v");
	show_machine = opt.first;


	acm pmm;

	set<string> words;
	istringstream line;
	string tmp;
	while ( !ifs.eof() ) {
		std::getline(ifs, tmp);
		if ( tmp.empty() )
			break;
		line.str(tmp);
		line.clear();
		while ( !line.eof() ) {
			line >> tmp;
			if ( words.size() < wordcount_max )
				words.insert(tmp);
		}
	}
	ifs.close();
	cout << "Got " << words.size() << " words: " << endl;
	if ( show_machine ) {
		for(auto tmp : words) {
			cout << tmp << ", ";
		}
		cout << endl << endl;
	}
	time_t sw = clock();
	pmm.addPatterns(words);
	sw = clock() -sw;
	if ( show_machine )
		cout << pmm << endl << endl;
	cout << "took " << sw / (double) CLOCKS_PER_SEC << " sec." << endl;

	cout << "proceed?" << endl;
	std::getline(cin, tmp);

	for ( auto occurr : pmm.search("bandman abandond") ) {
		cout << occurr.second << "@" << occurr.first << "," << endl;
	}
	cout << endl;

	cout << "bye." << endl << endl;
	return 0;
}
