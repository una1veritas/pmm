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

typedef int32_t int32;
typedef uint32_t uint32;
typedef uint16_t uint16;

class ACMachine {
public:
	typedef int32 state;
	typedef uint16 alphabet;
	typedef uint32 occurrence;

private:

	const static state initialState = 0;

private:
	std::set<const std::string> patterns;
	std::vector<std::map<alphabet,state>> transitions;
	std::vector<state> failures;
	std::vector<std::set<const std::string *> > output;

	state current;

public:
	ACMachine(void);

	void setupInitialState(void);

	uint32 size() { return transitions.size(); }

	bool transfer(const alphabet & c);

	// add patt to the trie and output of the destination state.
	state addPath(const std::string & patt);
	bool addOutput(const std::string & patt);

	std::vector<std::pair<uint32,const std::string &> > search(const std::string & pattern);

	std::ostream & printOn(std::ostream & out) const;

	friend std::ostream & operator<<(std::ostream & out, const ACMachine & acm) {
		return acm.printOn(out);
	}
};



#endif /* ACMACHINE_H_ */
