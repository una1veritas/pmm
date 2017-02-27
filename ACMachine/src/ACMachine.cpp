/*
 * ACMachine.cpp
 *
 *  Created on: 2017/02/27
 *      Author: sin
 */

#include <iostream>

#include "ACMachine.h"

/*
	std::set<const std::string> patterns;
	std::vector<std::map<alphabet,state> &> transitions;
	std::vector<state> failures;
	std::vector<std::set<const std::string *> > output;

	state current;
 *
 */
ACMachine::ACMachine(void) {
	setupInitialState();
	current = initialState;
	patterns.clear();
}

void ACMachine::setupInitialState(void) {
	transitions.clear();
	transitions.push_back(std::map<alphabet,state>());
	failures.clear();
	failures.push_back(-1);
	// -1 is shadow initial state indicating eating up one symbol on transition.
	output.clear();
	output.push_back(std::set<const std::string *>());
	output[initialState].clear();
}

bool ACMachine::transfer(const alphabet & c) {
	std::map<alphabet,state>::iterator itp;
	itp = transitions[current].find(c);
	if ( itp == transitions[current].end() )
		return false;
	current = itp->second;
	return true;
}

ACMachine::state ACMachine::addPath(const std::string & patt) {
	int pos;
	state nwstate;

	current = initialState;
	for(pos = 0; pos < patt.length(); ++pos) {
		if ( !transfer(patt[pos]) ) {
			nwstate = transitions.size();
			transitions.push_back(std::map<alphabet,state>());
			(transitions[current])[patt[pos]] = nwstate;
			failures.push_back(0);
			output.push_back(std::set<const std::string *>());
			current = nwstate;
		}
	}
	return current;
}

bool ACMachine::addOutput(const std::string & patt) {
	std::pair<std::set<const std::string>::iterator, bool> result;
	result = patterns.insert(patt);
	const std::string & orgstr = *result.first;
	if ( result.second ) {
		output[current].insert( &orgstr );
		return true;
	}
	return false;
}

std::vector<std::pair<uint32, const std::string &> > ACMachine::search(const std::string & pattern) {
	std::vector<std::pair<uint32, const std::string &> > occurrs;

	return occurrs;
}

std::ostream & ACMachine::printOn(std::ostream & out) const {
	out << "ACMachine() ";
	return out;
}
