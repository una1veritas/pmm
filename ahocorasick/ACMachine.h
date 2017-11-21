//=======================================
// ACMachine.h
//=======================================


#ifndef ACMACHINE_H_
#define ACMACHINE_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <chrono>
#include <cinttypes>


#ifndef TYPEDEF
#define TYPEDEF
typedef int32_t int32;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef size_t position;
#endif

class ACMachine {
public:
	typedef uint32 state;
	typedef uint16 alphabet;

	typedef const std::vector<alphabet> ustring;

	typedef std::pair<state, state> fstates;
	typedef std::vector<fstates> failurestates;
	typedef std::vector<state> outstates;

private:
	std::vector<std::map<alphabet, state>> transitions;
	std::vector<state> failure;
	std::vector<std::set<position> > output;
	std::vector<std::vector<state>> inv_failure;

	state current;

	// class constants
	const static state initial_state = 0;

private:
	std::ostream & printStateOn(std::ostream & out, state i, const std::string & pathstr) const;

	void setupInitialState(void);

	bool transfer(const alphabet & c, const bool ignore_case = false);
	//	state transition(const state s, const alphabet c);

	state initialState() const { return initial_state; }

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
		for (auto str : strset) {
			//std::cout << str << std::endl;
			resetState();
			addPath(str);
			addOutput(str);
		}
		addFailures();
	}

	template <typename T>
	int searchstateposition(const T patt[]);
	template <typename T>
	int searchstateposition(const T & patt);

	template <typename T>
	void addFailure(int i, const T & patt);
	template <typename T>
	void addFailure(int i, const T patt[]);

	template <typename T>
	void renewfunction(int i, const T & patt);
	template <typename T>
	void renewfunction(int i, const T patt[]);


	template <typename C>
	void dynamicaddPatterns(const C & str) {
	//void dynamicaddPatterns(std::string & str) {
	
		  int position;

		  position = searchstateposition(str);
		  
		  if(position != str.size()){
		    addPath(str);
		    addOutput(str);
		    addFailure(position, str);

		    renewfunction(position, str);
		  }
		  else{

		  }
		
	}

	template <typename T>
	void addfailurestates(failurestates fst, const T str[]){
	  const std::string s(str);
	  return addfailurestates(fst, s);
	}

	template <typename T>
	void addfailurestates(failurestates fst, const T & str){
		std::vector<state> tmp;
		resetState();
		tmp.push_back(current);
		if (!fst.empty()){
		  for (int i = 0; i < str.length(); i++){
				transfer(str[i]);
				tmp.push_back(current);
			}
			for (auto st : fst) {
				failure[st.first] = tmp[st.second];
				inv_failure[tmp[st.second]].push_back(st.first);
			}
		}
		
	}


	template <typename T>
	void addoutstates(outstates ost, const T str[]){
	  const std::string s(str);
	  return addoutstates(ost, s);
	}

	//void addoutstates(outstates ost, std::string str){
	template <typename T>
	void addoutstates(outstates ost, const T & str){
		if (!ost.empty()){
			for (auto st : ost){
				output[st].insert(str.size());
			}
		}
	}


	int statesize() {
		return transitions.size();
	}


	std::vector<std::pair<position, const std::string>>
		search(const std::string & text);
	bool read(const alphabet & c, const bool ignore_case = false);

	std::ostream & printOn(std::ostream & out) const;

	friend std::ostream & operator<<(std::ostream & out, const ACMachine & acm) {
		return acm.printOn(out);
	}

};

typedef ACMachine acm;


#endif /* ACMACHINE_H_ */
