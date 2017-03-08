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

#include <unistd.h>

using namespace std;


struct commandargs {
	char * const *argv;
	const int argc;
	const char * optformat;

	commandargs(const int argc, char * const * argv, const char * formstr)
	: argv(argv), argc(argc), optformat(formstr) { }

	int count() const { return argc - 1; }
	const char * arg(const unsigned int i) const {
		if ( i < count() )
			return argv[i+1];
		return NULL;
	}

	std::pair<bool,const char*> getopt(const char opt, const unsigned int item = 0) {
		char c;
	    opterr = 0; //getopt()のエラーメッセージを無効にする。
	    optind = 1;
	    while ((c = ::getopt(argc, argv, optformat)) != -1) {
	        //コマンドライン引数のオプションがなくなるまで繰り返す
	    	if ( c == opt ) {
	    		return std::pair<bool,const char*>(true,optarg);
	    	}
	    }
	    for(unsigned int i = 0; optind + i < argc; ++i) {
	    	if ( i == item )
	    		return std::pair<bool,const char*>(true,argv[optind+i]);
	    }
		return std::pair<bool,const char*>(false,NULL);
	}
};

#endif /* COMMANDARGS_H_ */
