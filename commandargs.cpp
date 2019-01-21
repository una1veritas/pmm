/*
* commandargs.cpp
*
*  Created on: 2017/03/10
*      Author: sin
*/

#include <unistd.h>
/*
* int getopt(int argc, char * const argv[], const char *optstring);
* extern char *optarg;
* extern int optind, opterr, optopt;
*/
#include "commandargs.h"

void commandargs::getopt(const int argc, char * const * argv, const char * optstr) {
	char c;
	opterr = 0; //getopt()�̃G���[���b�Z�[�W�𖳌��ɂ���B
	while ((c = ::getopt(argc, argv, optstr)) != -1) {
		//�R�}���h���C�������̃I�v�V�������Ȃ��Ȃ�܂ŌJ��Ԃ�
		opts[c] = optarg;
	}
	for (int i = optind; i < argc; i++) {
		args.push_back(argv[i]);
	}
}

std::pair<bool, const char*> commandargs::opt(const char c) const {
	std::map<const char, const char *>::const_iterator itr = opts.find(c);
	if (itr == opts.end())
		return std::pair<bool, const char*>(false, NULL);
	return std::pair<bool, const char*>(true, itr->second);
}
