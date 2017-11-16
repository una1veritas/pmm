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
#include <cmdargs.h>

#include <time.h>

int main(const int argc, char * const * argv) {
	ifstream ifs;
	int wordcount_max = 0;
	bool show_machine = false;
	bool show_words = false;
	bool ignore_case = false;
	string target;
	ifstream targetfile;
	istream * targetinput;

	commandargs optargs(argc, argv, "n:p:ivw");

	pair<bool,const char*> opt;
	opt = optargs.opt('n');
	if ( opt.first ) {
		wordcount_max = atol(opt.second);
		cout << "wordcount_max = " << wordcount_max << ", ";
	}
	opt = optargs.opt('p');
	if ( opt.first ) {
		string fname = string(opt.second);
		cout << "fname (pattern file) = " << fname << ", ";
		ifs.open(fname);
	}
	opt = optargs.opt('i');
	ignore_case = opt.first;
	cout << "ignore case = " << ignore_case << ", ";
	opt = optargs.opt('v');
	show_machine = opt.first;
	cout << "show_machine = " << show_machine << ", ";
	opt = optargs.opt('w');
	show_words = opt.first;
	cout << "show_words = " << show_words << ", ";
	if ( optargs.arg_count() == 0 ) {
		cout << "targetinput = cin" << ", ";
		targetinput = &cin;
	} else {
		target = optargs.arg(0);
		cout << "targetinput (file name) = " << target;
		targetfile.open(target);
		if ( !targetfile ) {
			cerr << "open file " << target << " failed!" << endl;
			if ( ifs ) ifs.close();
			return EXIT_FAILURE;
		}
		cout << ", ";
		targetinput = &targetfile;
	}
	cout << endl;

	acm pmm;

	vector<string> words;
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
			if ( wordcount_max == 0 || words.size() < wordcount_max )
				words.push_back(tmp);
		}
	}
	ifs.close();
	cout << "Got " << words.size() << " words: " << endl;
	if ( show_words ) {
		for(auto tmp : words)
			cout << tmp << ", ";
		cout << endl << endl;
	}
	time_t sw = clock();
	pmm.addPatterns(words);
	sw = clock() - sw;

	words.clear();
	if ( show_machine )
		cout << pmm << endl << endl;
	cout << "took " << sw / (double) CLOCKS_PER_SEC << " sec." << endl;

	//cout << "proceed?" << endl;
	//std::getline(cin, tmp);

	pmm.resetState();
	position pos = 0;
	string swindow = "";
	ACMachine::alphabet c;
	while ( !targetinput->eof() ) {
		c = targetinput->get();
		swindow.push_back(c);
		if ( pmm.read(c, ignore_case) ) {
			//cout << strwd << endl;
			if ( !pmm.currentOutput().empty() ) {
				for(auto pattlen : pmm.currentOutput()) {
					cout << swindow.substr(swindow.length() - pattlen , pattlen) << " @ " << (pos - pattlen + 1) << ", ";
				}
				cout << endl;
			}
		} else {
			//cout << "initial state" << endl;
			swindow.clear();
		}
		pos++;
	}
	cout << endl;

	if ( targetfile ) targetfile.close();

	cout << "bye." << endl << endl;
	return 0;
}
