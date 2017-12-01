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
#include <map>

#include <cinttypes>

typedef int32_t int32;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef size_t position;

class ACMachine {
public:
	typedef int32 state;
	typedef uint16 alphabet;
	typedef const std::vector<alphabet> ustring;

	// class constants
	enum {
		alph_start = 0,
		alph_end = (alphabet) - 1,
	};
	enum {
		initial_state = 0,
		undef_state = (state) -1,
	};

	struct TransTable {
	private:
		const static alphabet TRANSFER_SIZE = 128;

	public:
		state transfer[TRANSFER_SIZE];

		TransTable(void) {
			for(alphabet i = 0; i < TRANSFER_SIZE; ++i)
				transfer[i] = undef_state;
		}

		state operator[](const alphabet c) const {
			return transfer[c];
		}

		state & operator[](const alphabet c) {
			return transfer[c];
		}

		struct const_iterator {
			const state * transfer;
			std::pair<alphabet,state> translink;

			const_iterator(const state * transf, const alphabet c, const state s) {
				transfer = transf;
				translink.first = c;
				translink.second = s;
			}

			const std::pair<alphabet,state> & operator *() const {
				return translink;
			}

			const_iterator & findFirstOrNext(const alphabet c) {
				alphabet i;
				for(i = c; i < TRANSFER_SIZE && transfer[i] == undef_state; ++i);
				if ( c < TRANSFER_SIZE ) {
					translink.first = i;
					translink.second = transfer[i];
					return *this;
				}
				translink.first = TRANSFER_SIZE;
				translink.second = undef_state;
				return *this;

			}

			const_iterator & operator++() {
				findFirstOrNext(translink.first+1);
				return *this;
			}

			bool notequals(const const_iterator & j) const {
				return translink.first != j.translink.first;
			}

			bool friend operator!=(const const_iterator & a, const const_iterator & b) {
				return a.notequals(b);
			}


		};

		const_iterator begin() const {
			return const_iterator(transfer, alph_start, undef_state).findFirstOrNext(alph_start);
		}

		const_iterator end() const {
			return const_iterator(transfer, TRANSFER_SIZE, undef_state);
		}

		const_iterator find(const alphabet c) const {
			return const_iterator(transfer, alph_start, undef_state).findFirstOrNext(c);
		}

	};

private:
	std::vector<TransTable> transitions;
	std::vector<state> failure;
	std::vector<std::set<position> > output;

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

	const std::set<position> & currentOutput() const { return output[current]; }

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
