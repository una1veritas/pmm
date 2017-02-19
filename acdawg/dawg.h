/*
 * dawg.h
 *
 *  Created on: 2017/02/19
 *      Author: sin
 */

#ifndef DAWG_H_
#define DAWG_H_

#include <string>
#include <vector>

#include "actrie.h"


//int NODE_NUM_DAWG;
//int DAWGTOAC_NUM;

struct WGNode {
	//dawgにおけるノードの番号
	int nodenum;
	WGNode *edges[SIGMA_SIZE];
	//edge_num[i]  0 = edgeなし 1 = primary-edgeあり 2 = secondary-edgeあり
	int edge_num[SIGMA_SIZE];
	WGNode *suff;
	Trie *dtoc;
	std::vector<WGNode*> ine;
	//ACのノードとdawgのノードの番号を一致させるための変数
	int isTrunk;
	//trunkノードかbranchノードかの判定 1ならtrunk、0ならbranch 初期値は0
	int torb;

	WGNode(const int nm) : nodenum(nm), suff(NULL), dtoc(NULL), isTrunk(0), torb(0) {
//		nodenum = NODE_NUM_DAWG;
//		NODE_NUM_DAWG++;
		for (int i = 0; i < SIGMA_SIZE; i++) {
			edges[i] = NULL;
		}
		for (int i = 0; i < SIGMA_SIZE; i++) {
			edge_num[i] = 0;
		}

	}

};

class DAWG {
	WGNode nroot;

	int NODE_NUM_DAWG;
	int DAWGTOAC_NUM;

public:
	DAWG(void) : nroot(0), NODE_NUM_DAWG(1), DAWGTOAC_NUM(0) { }

	WGNode & root() { return nroot; }
	WGNode * split(WGNode *parentnode, WGNode *childnode, char a);
	WGNode * update(WGNode *activenode, char a);
	std::vector<Trie *> getoutstates(std::string &str);
	std::vector<Trie *> getfailstates(std::string &str, int depth);
	void addString(ACTrie & actrie, const std::string & patt);

	int ac_num() { return DAWGTOAC_NUM; }
	int inc_ac_num() { return ++DAWGTOAC_NUM; }
};



#endif /* DAWG_H_ */
