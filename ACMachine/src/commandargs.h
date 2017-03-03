/*
 * commandargs.h
 *
 *  Created on: 2017/03/02
 *      Author: sin
 */

#ifndef COMMANDARGS_H_
#define COMMANDARGS_H_


#include <cstdlib>
#include <cstring>

using namespace std;


struct commandargs {
	const char **arg_vals;
	const int arg_count;

	commandargs(const int argc, const char * argv[]) : arg_vals(argv+1), arg_count(argc-1) { }

	const char * getarg(const int i) {
		if ( i < arg_count )
			return arg_vals[i];
		else
			return NULL;
	}

	const int count() { return arg_count; }

	std::pair<bool,const char*> getopt(const char * optstr) {
		int optlen = strlen(optstr);
		int pos = -1;
		for(int i = 0; i < arg_count; i++ ) {
			if ( strncmp(optstr, arg_vals[i],optlen) == 0 ) {
				pos = i;
				break;
			}
		}
		if ( pos == -1 ) {
			return std::pair<bool,const char*>(false, NULL);
		}
		if ( pos + 1 < arg_count )
			return std::pair<bool, const char*>(true, arg_vals[pos+1]);
		else
			return std::pair<bool, const char*>(true, NULL);
	}
};

#endif /* COMMANDARGS_H_ */
