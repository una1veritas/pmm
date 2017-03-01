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

typedef char * c_str;

class ACMachine {
public:
	typedef uint32 state;
	typedef uint16 alphabet;

private:
	std::vector<std::map<alphabet,state>> transition;
	std::vector<state> failure;
	std::vector<std::set<const std::string *> > output;

	state current;

	std::set<std::string> pattern; // set<const string> is not vaild for pure gcc (but ok for clang gcc)

// class constants

	const static state initial_state = 0;

public:
	ACMachine(void);

	void setupInitialState(void);

	uint32 size() const { return transition.size(); }

	bool transfer(const alphabet & c);

	state resetState() { return current = initial_state; }
	state currentState() { return current; }
	state initialState() { return initial_state; }

	// add patt to the trie and output of the destination state.
	template <typename T>
	state addPath(T const & patt, const uint32 & length);
	template <typename T>
	state addPath(const T * patt);
	template <typename T>
	state addPath(T const & patt);

	bool addOutput(const std::string & patt);
	void addFailures();

	std::vector<std::pair<uint32,const std::string &> > search(const std::string & pattern);

	std::ostream & printOn(std::ostream & out) const;

	friend std::ostream & operator<<(std::ostream & out, const ACMachine & acm) {
		return acm.printOn(out);
	}

};

typedef ACMachine acm;


#endif /* ACMACHINE_H_ */
