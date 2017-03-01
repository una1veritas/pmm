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

#include <cinttypes>

typedef int32_t int32;
typedef uint32_t uint32;
typedef uint16_t uint16;

class ACMachine {
public:
	typedef uint32 state;
	typedef uint16 alphabet;

	const static state initial_state = 0;

private:
	std::set<const std::string> pattern;
	std::vector<std::map<alphabet,state>> transition;
	std::vector<state> failure;
	std::vector<std::set<const std::string *> > output;

	state current;

public:
	ACMachine(void);

	void setupInitialState(void);

	uint32 size() const { return transition.size(); }

	bool transfer(const alphabet & c);

	state resetState() { return current = initial_state; }
	state currentState() { return current; }
	state initialState() { return initial_state; }

	// add patt to the trie and output of the destination state.
	state addPath(const std::string & patt);
	bool addOutput(const std::string & patt);
	void addFailures();

	std::vector<std::pair<uint32,const std::string &> > search(const std::string & pattern);

	/*
	std::ostream & printOn(std::ostream & out) const;

	friend std::ostream & operator<<(std::ostream & out, const ACMachine & acm) {
		return acm.printOn(out);
	}
	*/
};



#endif /* ACMACHINE_H_ */
