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
		undef_state = -1,
	};

	struct TransTable {
	private:
		const static alphabet TRANSTABLE_ARRAY_LIMIT = 128;

	public:
		state transfer[TRANSTABLE_ARRAY_LIMIT];

		TransTable(void) {
			for(alphabet i = 0; i < TRANSTABLE_ARRAY_LIMIT; ++i)
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
			alphabet label;
			state dststate;

			const_iterator(const state * transf, const alphabet c, const state s) {
				transfer = transf;
				label = c;
				dststate = s;
			}

			const alphabet & operator *() {
				return label;
			}

			const alphabet operator *() const {
				return label;
			}

			const_iterator & findOrNext(const alphabet c) {
				for(label = c; label < TRANSTABLE_ARRAY_LIMIT && transfer[label] == undef_state; ++label);
				if ( label < TRANSTABLE_ARRAY_LIMIT ) {
					dststate = transfer[label];
					return *this;
				}
				label = alph_end;
				dststate = undef_state;
				return *this;

			}

			const_iterator & operator++() {
				findOrNext(label + 1);
				return *this;
			}

			bool notequals(const const_iterator & j) const {
				return label != j.label;
			}

			bool friend operator!=(const const_iterator & a, const const_iterator & b) {
				return a.notequals(b);
			}


		};

		const_iterator begin() const {
			return const_iterator(transfer, alph_start, undef_state).findOrNext(alph_start);
		}

		const_iterator end() const {
			return const_iterator(transfer, alph_end, undef_state);
		}

		const_iterator find(const alphabet c) const {
			return const_iterator(transfer, alph_start, undef_state).findOrNext(c);
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
