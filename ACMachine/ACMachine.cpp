/*
 * ACMachine.cpp
 *
 *  Created on: 2017/02/27
 *      Author: sin
 */

#include "ACMachine.h"

#include <deque>

#include <cctype>

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
	resetState();
}

void ACMachine::setupInitialState(void) {
	states.clear();
	states.push_back(MachineState(initial_state));
	// failure to initial state from initial state eats up one symbol at transition.
}

/*
ACMachine::state ACMachine::transition(const state src, const alphabet c) {
	state dst = failure[src];
	const std::map<alphabet,state>::iterator & itr = transitions[src].find(c);
	if ( itr != transitions[src].end() )
		dst = itr->second;
	return dst;
}
*/

bool ACMachine::transfer(const alphabet & c, const bool ignore_case) {
	stateIndex itr;
	if ( ignore_case ) {
		itr = states[current].transitions[toupper(c)];
		if ( itr == MachineState::trans_fail )
			itr = states[current].transitions[tolower(c)];
	} else {
		itr = states[current].transitions[c];
	}
	if ( itr == MachineState::trans_fail ) {
		return false;
	}
	current = itr;
	return true;
}


// trace or create the path on trie from the current state
template <typename T>
ACMachine::stateIndex ACMachine::addPath(const T patt[]) {
	uint32 len;
	for(len = 0; patt[len] != 0; len++) {}
	return addPath(patt, len);
}

template <typename T>
ACMachine::stateIndex ACMachine::addPath(const T & patt) {
	return addPath(patt, patt.length());
}

template <typename T>
ACMachine::stateIndex ACMachine::addPath(const T & patt, const uint32 & length) {
	uint32 pos;
	stateIndex newIndex;

	for(pos = 0; pos < length; ++pos) {
		if ( !transfer(patt[pos]) ) {
			newIndex = size(); //the next state of the existing last state
			states.push_back(MachineState(newIndex));
			states[current].transitions[ (uint8) patt[pos] ] = newIndex;
			current = newIndex;
		}
	}
	return current;
}

template ACMachine::stateIndex ACMachine::addPath<char>(const char patt[]);
template ACMachine::stateIndex ACMachine::addPath<ACMachine::alphabet>(const ACMachine::alphabet patt[]);
template ACMachine::stateIndex ACMachine::addPath<std::string>(const std::string & patt);

template <typename T>
bool ACMachine::addOutput(const T & patt) {
	std::pair<std::set<position>::iterator,bool> res = states[current].output.insert( patt.length() );
	return res.second;
}

template <typename T>
	bool ACMachine::addOutput(const T patt[]) {
	const std::string p(patt);
	return addOutput(p);
}

template bool ACMachine::addOutput<char>(const char patt[]);
template bool ACMachine::addOutput<std::string>(const std::string & patt);

void ACMachine::addFailures() {
	std::deque<stateIndex> q;

	// for states whose distance from the initial state is one.
//	std::cout << "states within distance one: ";
	for(auto const & nxstate : states[initial_state].transitions ) {
		//const alphabet c = assoc.first;
		// if is neither an explicit failure, nor go-root-failure
		states[nxstate].failure  = initial_state;
		q.push_back(nxstate);
//		std::cout << nxstate << ", ";
	}
	std::cout << std::endl;

	// for states whose distance from the initial state is more than one.
	while ( !q.empty() ) {
		const stateIndex cstate = q.front();
		q.pop_front();
//		std::cout << std::endl << "cstate " << cstate << std::endl;

		// skips if == NULL
		for(alphabet c = 0; c <= MachineState::alph_limit; ++c ) {
			const stateIndex nxstate = states[cstate].transitions[c];
			q.push_back(nxstate);

//			std::cout << cstate << " -" << (char) c << "-> " << nxstate << std::endl;

			stateIndex fstate = states[cstate].failure;
//			std::cout << cstate << " ~~> " << fstate << " ";
			stateIndex itp;
			while (1) {
				itp = states[fstate].transitions[c];
				if ( itp == MachineState::trans_fail && fstate != initial_state ) {
					fstate = states[fstate].failure;
//					std::cout << " ~~> " << fstate << " ";
				} else {
					break;
				}
			}
			if ( itp == MachineState::trans_fail ) {
				states[nxstate].failure = initial_state;
			} else {
				states[nxstate].failure = itp;
				states[nxstate].output.insert(states[states[nxstate].failure].output.begin(), states[states[nxstate].failure].output.end());
			}
//			std::cout << std::endl << "set "<< nxstate << " ~~> " <<  failure[nxstate];
//			std::cout << std::endl;
		}
	}
}

std::vector<std::pair<position, const std::string> >
	ACMachine::search(const std::string & text) {
	std::vector<std::pair<position, const std::string> > occurrs;
	std::deque<alphabet> uring;

	uint32 pos = 0;
	resetState();
	uring.clear();
	while ( pos < text.size() ) {
		if ( transfer(text[pos]) ) {
			uring.push_back(text[pos]);
			if ( !states[current].output.empty() ) {
				for(std::set<position>::iterator it = states[current].output.begin();
						it != states[current].output.end(); it++) {
					const position patlen = *it;
					std::string patt(patlen, ' ');
					for(position i = 0; i < patlen; i++) {
						patt[i] = uring.at(uring.size() - patlen + i);
					}
					occurrs.push_back(std::pair<position,const std::string>(pos + *it + 1, patt));
				}
			}
			pos++;
		} else {
			// failure loop
			current = states[current].failure;
			if ( current == initialState() ) {
				pos++;
				//uring.push_back(text[pos]);
				uring.clear();
			} else {
				uring.pop_front();
			}
		}
	}
	return occurrs;
}

bool ACMachine::read(const alphabet & c, const bool ignore_case) {
	do {
		if ( transfer(c, ignore_case) )
			return true;
		current = states[current].failure;
	} while ( current != initialState() );
	return false;
}



std::ostream & ACMachine::printStateOn(std::ostream & out, stateIndex i, const std::string & pathstr) const {
	if ( i == current ) {
		out << "<" << i << ">";
	} else {
		out << i;
	}
	if ( states[i].output.size() > 0 ) {
		out << "{";
		for(std::set<position>::iterator it = states[i].output.begin();
				it != states[i].output.end(); ) {
			out << pathstr.substr(pathstr.length() - *it, *it);
			if ( ++it != states[i].output.end() )
				out << ", ";
		}
		out << "}";
	}
	out << "[";
	for(alphabet it = 0; it <= MachineState::alph_limit; it++) {
		if ( states[i].transitions[it] != MachineState::trans_fail )
			out << "'" << it << "'-> " << states[i].transitions[it] << ", ";
	}
	out << "~> " << states[i].failure;
	out << "], ";

	return out;
}



std::ostream & ACMachine::printOn(std::ostream & out) const {
	std::deque< std::pair<alphabet,stateIndex> > path;
	stateIndex curr;
	std::string str;

	out << "ACMachine(";
	path.push_back(std::pair<alphabet,stateIndex>(0, initial_state) );
	curr = initialState();
	str = "";
	printStateOn(out,curr,str);

	alphabet c;
	stateIndex tsta;
	for(c = 0; c <= MachineState::alph_limit; ++c) {
		tsta = states[curr].transitions[c];
		if ( tsta != MachineState::trans_fail )
			break;
	}
	if ( tsta != MachineState::trans_fail ) {
		path.push_back(std::pair<alphabet,stateIndex>(c,tsta) );
		curr = tsta;
	}

	while ( !path.empty() ) {
		if ( curr == path.back().second ) {
			// I'm on top.
			// print curr, then go to the first child if exist
			str.resize(path.size());
			for(position i = 0; i < path.size(); i++) {
				str[i] = path[i]->first;
			}
			printStateOn(out,curr, str);
			c = states[curr].transitions.begin(); // the first transition arc
		} else {
			// returned from the child that still on path top.
			// find next to path.back()
			c = path.back();
			path.pop_back(); // remove last edge
			++c; // the next transition arc
		}
		if ( c != states[curr].transitions.end()) {
			path.push_back(c); // replace with new edge
			curr = c->second;
			continue;
		} else {
			c = path.back();
			path.pop_back();
			if ( path.empty() ) // popped the dummy path to the initial state.
				continue;
			curr = path.back()->second;
			path.push_back(c);
			continue;
		}
	}

	out << ") ";
	return out;
}
