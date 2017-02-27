/*
 * ACMachine.h
 *
 *  Created on: 2017/02/27
 *      Author: sin
 */

#ifndef ACMACHINE_H_
#define ACMACHINE_H_

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>

#include <cinttypes>

typedef uint32_t uint32;
typedef uint16_t uint16;

class ACMachine {
	typedef uint32 state;
	typedef uint16 alphabet;
	typedef uint32 occurrence;

	struct tuple {
		state id;
		state failure;
		std::map<alphabet,state> go_to;
	};
private:
	std::set<std::string> keywords;
	std::vector<tuple> states;
	std::vector<std::set<std::string &> > output;
public:
	ACMachine(void);
	ACMachine(const std::string & pattern);

	std::vector<std::pair<uint32,const std::string &> > search(const std::string & pattern);

};



#endif /* ACMACHINE_H_ */
