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
	typedef int32 state;
	typedef uint8 alphabet;
//	typedef const std::vector<alphabet> ustring;

	static const uint32 alphabet_size = 1<<(sizeof(alphabet)*8);
	static const uint32 failure_index = alphabet_size;
	struct TrieNode {
		state table[alphabet_size+1];
		std::vector< position > output;
	};

	// class constants
	enum {
		alph_end = (alphabet) - 1,
	};
	enum {
		initial_state = 0,
		undef_state = (state) -1,
	};

private:
	std::vector<ACMachine::TrieNode> transitions;
	state current;

private:
	std::ostream & printStateOn(std::ostream & out, state i, const std::string & pathstr) const;

	void setupInitialState(void);
	state initialState() const { return initial_state; }
	bool transfer(const alphabet & c, const bool ignore_case = false);

	state transition(const state s, const alphabet c) const;

public:
	ACMachine(void);

	uint32 size() const { return transitions.size(); }

	state resetState() { return current = initial_state; }
	state currentState() const { return current; }

	bool atInitialState() const { return current == initial_state; }

	const std::vector<position> & currentOutput() const { return transitions[current].output; }

	// add patt to the trie and output of the destination state.
	template <typename T>
		state addPath(const T & patt, const uint32 & length);
	template <typename T>
		state addPath(const T patt[]);
	template <typename T>
		state addPath(const T & patt);

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
