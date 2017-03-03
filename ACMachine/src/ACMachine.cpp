/*
 * ACMachine.cpp
 *
 *  Created on: 2017/02/27
 *      Author: sin
 */

#include "ACMachine.h"

#include <queue>

/*
	std::set<std::string> patterns;
	std::vector<std::map<alphabet,state>> transitions;
	std::vector<state> failures;
	std::vector<std::set<const std::string *> > output;

	state current;
 *
 */

ACMachine::ACMachine(void) {
	setupInitialState();
	pattern.clear();
	resetState();
}

void ACMachine::setupInitialState(void) {
	transitions.clear();
	transitions.push_back(std::map<alphabet,state>());
	failure.clear();
	failure.push_back(initialState());
	// failure to initial state from initial state eats up one symbol at transition.
	output.clear();
	output.push_back(std::set<const std::string *>());
	output[initialState()].clear();
}


ACMachine::state ACMachine::transition(const state src, const alphabet c) {
	state dst = failure[src];
	const std::map<alphabet,state>::iterator & itr = transitions[src].find(c);
	if ( itr != transitions[src].end() )
		dst = itr->second;
	return dst;
}

bool ACMachine::transfer(const alphabet & c) {
	std::map<alphabet,state>::iterator itr = transitions[current].find(c);
	if ( itr == transitions[current].end() ) {
		return false;
	}
	current = itr->second;
	return true;
}


// trace or create the path on trie from the current state
template <typename T>
ACMachine::state ACMachine::addPath(const T patt[]) {
	uint32 len;
	for(len = 0; patt[len] != 0; len++) {}
	return addPath(patt, len);
}

template <typename T>
ACMachine::state ACMachine::addPath(const T & patt) {
	return addPath(patt, patt.length());
}

template <typename T>
ACMachine::state ACMachine::addPath(const T & patt, const uint32 & length) {
	uint32 pos;
	state newstate;

	for(pos = 0; pos < length; ++pos) {
		if ( !transfer(patt[pos]) ) {
			newstate = transitions.size(); //the next state of the existing last state
			transitions.push_back(std::map<alphabet,state>());
			(transitions[current])[patt[pos]] = newstate;
			failure.push_back(initialState());
			output.push_back(std::set<const std::string *>());
			current = newstate;
		}
	}
	return current;
}

template ACMachine::state ACMachine::addPath<char>(const char patt[]);
template ACMachine::state ACMachine::addPath<ACMachine::alphabet>(const ACMachine::alphabet patt[]);
template ACMachine::state ACMachine::addPath<std::string>(const std::string & patt);

template <typename T>
bool ACMachine::addOutput(const T & patt) {
	const std::pair<std::set<std::string>::iterator, bool> & result = pattern.insert(patt);
	const std::string & orgstr = *result.first;
	if ( result.second ) {
		output[current].insert( &orgstr );
		return true;
	}
	return false;
}

template <typename T>
	bool ACMachine::addOutput(const T patt[]) {
	const std::string p(patt);
	return addOutput(p);
}

template bool ACMachine::addOutput<char>(const char patt[]);
template bool ACMachine::addOutput<std::string>(const std::string & patt);

void ACMachine::addFailures() {
	std::queue<state> q;

	// for states whose distance from the initial state is one.
//	std::cout << "states within distance one: ";
	for(auto const & assoc : transitions[initial_state] ) {
		//const alphabet c = assoc.first;
		const state nxstate = assoc.second;
		// if is neither an explicit failure, nor go-root-failure
		failure[nxstate]  = initial_state;
		q.push(nxstate);
//		std::cout << nxstate << ", ";
	}
	std::cout << std::endl;

	// for states whose distance from the initial state is more than one.
	while ( !q.empty() ) {
		const state cstate = q.front();
		q.pop();
//		std::cout << std::endl << "cstate " << cstate << std::endl;

		// skips if == NULL
		for(auto const & assoc : transitions[cstate] ) {
			const alphabet c  = assoc.first;
			const state nxstate = assoc.second;
			q.push(nxstate);

//			std::cout << cstate << " -" << (char) c << "-> " << nxstate << std::endl;

			state fstate = failure[cstate];
//			std::cout << cstate << " ~~> " << fstate << " ";
			std::map<alphabet,state>::iterator itp;
			while (1) {
				itp = transitions[fstate].find(c);
				if ( itp == transitions[fstate].end() && fstate != initial_state ) {
					fstate = failure[fstate];
//					std::cout << " ~~> " << fstate << " ";
				} else {
					break;
				}
			}
			if ( itp == transitions[fstate].end() ) {
				failure[nxstate] = initial_state;
			} else {
				failure[nxstate] = itp->second;
				output[nxstate].insert(output[failure[nxstate]].begin(),output[failure[nxstate]].end());
			}
//			std::cout << std::endl << "set "<< nxstate << " ~~> " <<  failure[nxstate];
//			std::cout << std::endl;
		}
	}
}

std::vector<std::pair<uint32, const std::string &> >
	ACMachine::search(const std::string & pattern) {
	std::vector<std::pair<uint32, const std::string &> > occurrs;

	uint32 pos = 0;
	resetState();
	while ( pos < pattern.size() ) {
		if ( transfer(pattern[pos]) ) {
			if ( !output[current].empty() ) {
				for(std::set<const std::string *>::iterator it = output[current].begin();
						it != output[current].end(); it++) {
					occurrs.push_back(std::pair<uint32,const std::string&>(pos - (*it)->length() + 1,**it));
				}
			}
			pos++;
		} else {
			// failure loop
			current = failure[current];
			if ( current == initialState() ) {
				pos++;
			}
		}
	}
	return occurrs;
}


std::ostream & ACMachine::printOn(std::ostream & out) const {
	out << "ACMachine(";
	for(state i = 0; i < size(); ++i ) {
		if ( i == current ) {
			out << "<" << i << ">";
		} else {
			out << i;
		}
		if ( output[i].size() > 0 ) {
			out << "{";
			for(std::set<const std::string *>::iterator it = output[i].begin();
					it != output[i].end(); ) {
				out << "'" << **it << "'";
				if ( ++it != output[i].end() )
					out << ", ";
			}
			out << "} ";
		}
		out << "[";
		for(std::map<alphabet,state>::const_iterator it = transitions[i].begin();
				it != transitions[i].end(); it++) {
			out << "'" << (char)it->first << "'-> " << it->second << ", ";
		}
		out << "~> " << failure[i];
		out << "], ";
	}
	out << ") ";
	return out;
}
