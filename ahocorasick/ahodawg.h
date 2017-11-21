//==========================================
// ahodawg.h
//==========================================

#ifndef AHODAWG_H_
#define AHODAWG_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <deque>
#include <time.h>
#include <chrono>

#include <cinttypes>

#include "ACMachine.h"
#include "dawg.h"

class AhoDawg {
private:
	ACMachine ac;
	DAWG da;



public:
	AhoDawg(void);

	typedef uint32 state;
	typedef uint16 alphabet;

	template <typename C>
	void BuildAhoandDawg(const C & strset){
		ac.addPatterns(strset);
		da.buildingdawg(strset);
	}

	int statesize() {
		return ac.statesize();
	}


	template <typename C>
	  //void DynamicBAD(const C & strset, int &totalstatesize, int addcount, std::ofstream & ofs, int words_size){
	  void DynamicBAD(const C & strset){
	//void DynamicBAD(std::string  & str) {
	  for(auto str : strset){
	    std::cout << "dynamic bad" << std::endl;

		int position;
		position = ac.searchstateposition(str);

		ac.addPath(str);
		ac.addOutput(str);

		std::vector<DAWG::fstates> failstates;
		//firststate: from /secondstate: to
		failstates = da.getFailStates(position, str);
		ac.addfailurestates(failstates, str);

		ac.addFailure(position, str);

		std::vector<DAWG::state> outstates;
		outstates = da.getOutStates(str);
		ac.addoutstates(outstates, str);

		da.builddawg(str);
	  }
	}

};

typedef AhoDawg ahdawg;

#endif 
