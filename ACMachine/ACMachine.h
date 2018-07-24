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
#include <array>
#include <set>
//#include <map>

#include <cinttypes>

typedef int32_t int32;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef size_t position;

class ACMachine {
public:
	typedef int32 state_index;
	typedef uint8 alphabet;
//	typedef const std::vector<alphabet> ustring;

	static const uint32 alphabet_size = 1<<(sizeof(alphabet)*8);

	// class constants
	enum {
		alph_end = (alphabet) - 1,
	};

private:
	struct State {
		state_index trans[alphabet_size];
		state_index failure;
		std::vector< position > output;

		enum {
			initial = 0,
			undefined = (state_index) -1,
		};

		State() {
			memset(trans, State::undefined, alphabet_size);
			failure = initial;
			output.clear();
		}
	};
	std::vector<State> states;
	state_index current;

private:
	std::ostream & printStateOn(std::ostream & out, state_index i, const std::string & pathstr) const;

	void setupInitialState(void);
	state_index initialState() const { return State::initial; }
	bool transfer(const alphabet & c, const bool ignore_case = false);

	state_index transition(const state_index s, const alphabet c) const;

public:
	ACMachine(void);

	uint32 size() const { return states.size(); }

	state_index resetState() { return current = State::initial; }
	state_index currentState() const { return current; }

	bool atInitialState() const { return current == State::initial; }

	const std::vector<position> & currentOutput() const { return states[current].output; }

	// add patt to the trie and output of the destination state.
	template <typename T>
		state_index addPath(const T & patt, const uint32 & length);
	template <typename T>
		state_index addPath(const T patt[]);
	template <typename T>
		state_index addPath(const T & patt);

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

#endif /* ACMACHINE_H_ */
