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
	transitions.clear();
	//transitions.push_back(std::map<alphabet,state>());
	transitions.push_back(TransTable());
	failure.clear();
	failure.push_back(initialState());
	// failure to initial state from initial state eats up one symbol at transition.
	output.clear();
	output.push_back(std::set<position>());
	output[initialState()].clear();
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

ACMachine::alphastate ACMachine::transition(const ACMachine::state s, const ACMachine::alphabet c) const {
/*
	std::map<alphabet,state>::const_iterator citr = transitions[s].find(c);
	if ( citr == transitions[s].end() ) {
		return alphastate(alph_end,failure_state);
	}
	return alphastate(citr->first,citr->second);
	*/
	return transitions[s][c];
}

bool ACMachine::transfer(const alphabet & c, const bool ignore_case) {
	alphastate aspair;
	if ( ignore_case ) {
		aspair = transition(current,toupper(c));
		if ( aspair.second == failure_state )
			aspair = transition(current,tolower(c));
	} else {
		aspair = transition(current,c);
	}
	if ( aspair.second == failure_state ) {
		return false;
	}
	current = aspair.second;
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
			newstate = size(); //the next state of the existing last state
			//transitions.push_back(std::map<alphabet,state>());
			transitions.push_back(TransTable());
			//(transitions[current])[patt[pos]] = newstate;
			transitions[current].define(patt[pos],newstate);
			failure.push_back(initialState());
			output.push_back(std::set<position>());
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
	std::pair<std::set<position>::iterator,bool> res = output[current].insert( patt.length() );
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
	std::deque<state> q;

	// for states whose distance from the initial state is one.
//	std::cout << "states within distance one: ";
	for(auto const & assoc : transitions[initial_state] ) {
		//const alphabet c = assoc.first;
		const state nxstate = assoc.second;
		// if is neither an explicit failure, nor go-root-failure
		failure[nxstate]  = initial_state;
		q.push_back(nxstate);
//		std::cout << nxstate << ", ";
	}
//	std::cout << std::endl;

	// for states whose distance from the initial state is more than one.
	while ( !q.empty() ) {
		const state cstate = q.front();
		q.pop_front();
//		std::cout << std::endl << "cstate " << cstate << std::endl;

		// skips if == NULL
		for(auto const & assoc : transitions[cstate] ) {
			const alphabet c  = assoc.first;
			const state nxstate = assoc.second;
			q.push_back(nxstate);

//			std::cout << cstate << " -" << (char) c << "-> " << nxstate << std::endl;

			state fstate = failure[cstate];
//			std::cout << cstate << " ~~> " << fstate << " ";
			alphastate aspair;
			while (1) {
				aspair = transition(fstate,c);
				if ( aspair.second == failure_state && fstate != initial_state ) {
					fstate = failure[fstate];
					continue;
//					std::cout << " ~~> " << fstate << " ";
				}
				break;
			}
			if ( aspair.second == failure_state ) {
				failure[nxstate] = initial_state;
			} else {
				failure[nxstate] = aspair.second;
				output[nxstate].insert(output[failure[nxstate]].begin(),output[failure[nxstate]].end());
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
			if ( !output[current].empty() ) {
				for(std::set<position>::iterator it = output[current].begin();
						it != output[current].end(); it++) {
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
			current = failure[current];
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
		current = failure[current];
	} while ( current != initialState() );
	return false;
}



std::ostream & ACMachine::printStateOn(std::ostream & out, state i, const std::string & pathstr) const {
	if ( i == current ) {
		out << "<" << i << ">";
	} else {
		out << i;
	}
	if ( output[i].size() > 0 ) {
		out << "{";
		for(std::set<position>::iterator it = output[i].begin();
				it != output[i].end(); ) {
			out << pathstr.substr(pathstr.length() - *it, *it);
			if ( ++it != output[i].end() )
				out << ", ";
		}
		out << "}";
	}
	out << "[";
	for(std::map<alphabet,state>::const_iterator it = transitions[i].begin();
			it != transitions[i].end(); it++) {
		out << "'" << (char)it->first << "'-> " << it->second << ", ";
	}
	out << "~> " << failure[i];
	out << "], ";

	return out;
}


std::ostream & ACMachine::printOn(std::ostream & out) const {
	std::deque<std::pair<alphabet,state> > path;
	state curr;
	std::string str;

	// dummy arc to the initial state (from nothing).
	std::pair<alphabet,state> dummy(alph_end,initial_state);
	out << "ACMachine(";
	path.push_back(dummy);
	curr = initial_state;
	str = "";
	//printStateOn(out,curr,str);

	alphabet nextlabel;
	while ( !path.empty() ) {
		if ( curr == path.back().second ) {
			// I'm on top.
			// print curr, then go to the first child if exist
			str.resize(path.size());
			for(position i = 0; i < path.size(); i++) {
				str[i] = path[i].first;
			}
			printStateOn(out,curr, str);
			; // the first transition arc
			if ( transitions[curr].begin() != transitions[curr].end() ) {
				nextlabel = transitions[curr].begin()->first;
			} else {
				nextlabel = alph_end;
			}
		} else {
			// returned from the child that still on path top.
			// find next to path.back()
			nextlabel = path.back().first;
			path.pop_back(); // remove last edge
			curr = path.back().second;
			std::map<alphabet,state>::const_iterator it = transitions[curr].find(nextlabel);
			++it;
			// the next transition arc
			if ( it != transitions[curr].end() ) {
				nextlabel = it->first;
			} else {
				nextlabel = alph_end;
			}
		}
		if ( nextlabel != alph_end ) {
			state nexstate = transitions[curr].find(nextlabel)->second;
			path.push_back(std::pair<alphabet,state>(nextlabel,nexstate)); // replace with new edge
			curr = nexstate;
		} else {
			std::pair<alphabet,state> aspair = path.back();
			path.pop_back();
			if ( path.empty() ) // popped the dummy path to the initial state.
				break;
			curr = path.back().second;
			path.push_back(aspair);
		}
	}

	out << ") ";
	return out;
}
