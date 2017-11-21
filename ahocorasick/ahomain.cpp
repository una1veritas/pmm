//============================================================================
//ahomain.cpp
//============================================================================

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;

#include "ACMachine.h"
#include "commandargs.h"

#include <time.h>
#include <iomanip>
#include <cstdlib>

typedef uint32 state;
typedef uint16 alphabet;

int main(const int argc, char * const * argv) {
	ifstream ifs, addifs;
	int wordcount_max = 0;
	bool show_machine = false;
	bool show_words = false;
	bool ignore_case = false;
	string target;
	ifstream targetfile;
	istream * targetinput;

	commandargs optargs(argc, argv, "n:f:a:ivw");

	pair<bool, const char*> opt;
	opt = optargs.opt('n');
	if (opt.first) {
		wordcount_max = atol(opt.second);
		cout << "wordcount_max = " << wordcount_max << ", ";
	}
	opt = optargs.opt('f');
	if (opt.first) {
		string fname = string(opt.second);
		cout << "fname (pattern file) = " << fname << ", ";
		ifs.open(fname);
	}
	opt = optargs.opt('a');
	if (opt.first) {
		string addfname = string(opt.second);
		cout << "addfname (pattern file) = " << addfname << ", ";
		addifs.open(addfname);
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
	if (optargs.arg_count() == 0) {
		cout << "targetinput = cin" << ", ";
		targetinput = &cin;
	}
	else {
		target = optargs.arg(0);
		cout << "targetinput (file name) = " << target;
		targetfile.open(target);
		if (!targetfile) {
			cerr << "open file " << target << " failed!" << endl;
			if (ifs) ifs.close();
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
	while (!ifs.eof()) {
		std::getline(ifs, tmp);
		if (tmp.empty())
			break;
		line.str(tmp);
		line.clear();
		while (!line.eof()) {
			line >> tmp;
			if (wordcount_max == 0 || words.size() < wordcount_max)
				words.push_back(tmp);
		}
		if (words.size() == wordcount_max)
			break;
	}
	cout << "Got " << words.size() << " words: " << endl;
	if (show_words) {
		for (auto tmp : words)
			cout << tmp << ", ";
		cout << endl << endl;
	}
	pmm.addPatterns(words);

	words.clear();
	cout << "build end" << endl;




	//
	//dynamic aho-corasick
	//

	vector<string> addwords;

	istringstream addline;
	string addtmp;

	int state_width = 10000;

	int counter_max = 450000 / state_width;

	int nextstatescount = 1;

	vector<int> statesize_vec;
	vector<int> wordnum_vec;
	vector<int> time_vec;


	while (!addifs.eof()) {
		std::getline(addifs, addtmp);
		if (addtmp.empty())
			break;
		addline.str(addtmp);
		addline.clear();
		while (!addline.eof()) {
			addline >> addtmp;
			addwords.push_back(addtmp);
			addline >> addtmp;
		}

	}
	cout << "Got " << addwords.size() << " add words: " << endl;
	if (show_words) {
		for (auto tmp : addwords)
			cout << tmp << ", ";
		cout << endl << endl;
	}

	int time = 0;
	struct timeval start, end;
	gettimeofday(&start, NULL);
	for (int i = 0; i < addwords.size(); i++) {

		pmm.dynamicaddPatterns(addwords[i]);

		if (ad.statesize() >= (nextstatescount * state_width)) {
			gettimeofday(&end, NULL);
			time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
			time_vec.push_back(time);
			wordnum_vec.push_back(i);
			statesize_vec.push_back(ad.statesize());
			//cout << nextstatescount << endl;

			nextstatescount++;
		}

		if (nextstatescount > counter_max)
			break;
	}



	for (int i = 0; i < (nextstatescount - 1); i++) {
		cout << setw(3) << i + 1 << ",";
		coutv << setw(6) << statesize_size[i] << ",";
		cout << setw(6) << wordnum_size[i] << ",";
		cout << setw(5) << time_size[i] << ",";
		if (i == 0) {
			cout << setw(6) << statesize_size[i] << ",";
			cout << setw(6) << wordnum_size[i] << ",";
			cout << setw(5) << time_size[i] << ",";
			cout << setw(6) << (double)time_size[i] / wordnum_size[i];
		}
		if (i != 0) {
			cout << setw(6) << statesize_size[i] - statesize_size[i - 1] << ",";
			cout << setw(5) << wordnum_size[i] - wordnum_size[i - 1] << ",";
			cout << setw(5) << time_size[i] - time_size[i - 1] << ",";
			cout << setw(6) << (double)(time_size[i] - time_size[i - 1]) / (wordnum_size[i] - wordnum_size[i - 1]);
		}
		cout << endl;
	}
	


		//if (targetfile) targetfile.close();
		//if (targetinput2) targetinput2.close();
	

		cout << "bye." << endl << endl;
		return 0;
	
}
