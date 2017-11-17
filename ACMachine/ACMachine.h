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

typedef uint8_t uint8;
typedef int32_t int32;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef size_t position;

class ACMachine {
public:
	typedef uint32 stateIndex;
	typedef uint16 alphabet;

	typedef const std::vector<alphabet> ustring;

	// class constants
		const static stateIndex initial_state = 0;

private:
	struct MachineState {
		stateIndex id;
		stateIndex transitions[ 1<<8 ];
		stateIndex failure;
		std::set<position> output;

		static const stateIndex trans_fail = initial_state;
		static const alphabet alph_limit = (1<<8) - 1;
		MachineState(const stateIndex sindex) {
			id = sindex;
			for(alphabet c = 0; c < alph_limit; ++c)
				transitions[c] = trans_fail;
			failure = initial_state;
			output.clear();
		}
	};
	std::vector<MachineState> states;
	/*
	std::vector<std::map<alphabet,state>> transitions;
	std::vector<state> failure;
	std::vector<std::set<position> > output;
	*/
	stateIndex current;

private:
	std::ostream & printStateOn(std::ostream & out, stateIndex i, const std::string & pathstr) const;

	void setupInitialState(void);

	bool transfer(const alphabet & c, const bool ignore_case = false);
//	state transition(const state s, const alphabet c);

	stateIndex initialState() const { return initial_state; }

public:
	ACMachine(void);


	uint32 size() const { return states.size(); }


	stateIndex resetState() { return current = initial_state; }
	stateIndex currentState() const { return current; }

	bool atInitialState() const { return current == initial_state; }

	const std::set<position> & currentOutput() const { return states[current].output; }

	// add patt to the trie and output of the destination state.
	template <typename T>
		stateIndex addPath(const T & patt, const uint32 & length);
	template <typename T>
		stateIndex addPath(const T patt[]);
	template <typename T>
		stateIndex addPath(const T & patt);

	template <typename T>
		bool addOutput(const T & patt);
	template <typename T>
		bool addOutput(const T patt[]);

	void addFailures();

	template <typename C>
	void addPatterns(const C & strset) {
		for(auto str : strset ) {
			resetState();
			addPath(str);
			addOutput(str);
		}
		addFailures();
	}

	std::vector<std::pair<position,const std::string>>
	search(const std::string & text);
	bool read(const alphabet & c, const bool ignore_case = false);

	std::ostream & printOn(std::ostream & out) const;

	friend std::ostream & operator<<(std::ostream & out, const ACMachine & acm) {
		return acm.printOn(out);
	}

};

typedef ACMachine acm;


#endif /* ACMACHINE_H_ */
