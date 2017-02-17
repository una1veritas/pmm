#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <time.h>

#include <sstream>

using namespace std;

const int SIGMA_SIZE = 255;
int NODE_NUM_DAWG = 0;
int DAWGTOAC_NUM = 0;

struct Trie {
	//Trie *edges[SIGMA_SIZE];
	map<int, Trie *> edges;
	Trie *fail;
	set<string> out;
	int nodenum;
	int ine_num;

	Trie(const int id) : fail(NULL), nodenum(id), ine_num(0) {
		out.clear();
	}
};

// AC Machine by Trie
class ACTrie {
	Trie troot; // = Trie(0);
	int node_num;

	Trie * current;

public:
	ACTrie() : troot(0), node_num(1), current(&troot) {
		// rootの初期化
		for (int i = 0; i < SIGMA_SIZE; i++) {
			root().edges[i] = &root();
			root().fail = &root();
		}
	}

	unsigned int size() const {
		return node_num;
	}
	Trie & root(void) {	return troot; }
	bool isRoot(const Trie & t) const {
		return &t == &troot;
	}
	bool isRoot(const Trie * t) const {
		return t == &troot;
	}
	Trie * branch(Trie & node, int ch) const;
	inline Trie * branch(int ch) const { return branch(*current, ch); }

	void addBranch(Trie & node, int ch) {
		node.edges[ch] = new Trie(node_num);
		node_num++;
	}
	inline void addBranch(int ch) { return addBranch(*current, ch); }
	int addString(const string &str);

	void addPattern(const string & patt);

	void build(const std::vector<std::string>::iterator & start, const std::vector<std::string>::iterator & end);
	void scan(string & text, vector<pair<int,string>> & occurrs);


	void resetState(void) { current = &troot; }
	Trie & currentState(void) { return *current; }
	void transferState(int ch);
	void setState(Trie & node) { current = &node; }
};

Trie * ACTrie::branch(Trie & node, int ch) const {
	map<int, Trie *>::iterator it = node.edges.find(ch);
	pair<int,Trie*> entry = *it;
	if ( it == node.edges.end() )
		return NULL;
	return entry.second;
}

int ACTrie::addString(const string & str) {
	int extendLen = 0;
	resetState();
	for (unsigned int depth = 0; depth < str.size(); depth++) {
		Trie * next = branch(str[depth]);
		if ( next == NULL || isRoot(next) ) {
			addBranch(str[depth]); //node->edges[next] = new Trie();
			extendLen++;
		}
		transferState(str[depth]);
	}
	// assuming .out do not have the same string.
	currentState().out.insert(str);
	return extendLen;
}

/*
int ACTrie::addString2(Trie *node, const string &curString, int depth = 0, int depth2 = -1) {

	if (depth == curString.size()) {
		node->out.insert(curString);

		return depth2;
	}

	int next = curString[depth]; // - ' ';

	if (node->edges.find(next) == node->edges.end() || isRoot(*node->edges[next]) ) {
		addBranch(*node, next); //node->edges[next] = new Trie();
		if (depth2 == -1)
			depth2 = depth;
	}

	return addString2(node->edges[next], curString, depth + 1, depth2);
}
*/

void ACTrie::build(const vector<string>::iterator & start, const vector<string>::iterator & end) {
	std::queue<Trie *> q;

	// goto関数と状態の追加
	for(std::vector<string>::iterator it = start; it != end; it++ ) {
		addString(*it); //Goto(troot, reading_line);
	}

	// set up failures of root
	// Must to this before, because of the fact that every edge out of the root
	// must not be NULL.
	resetState();
//	for (int i = 0; i < SIGMA_SIZE; i++) {
	// skips if == NULL
	for(auto const & assoc : currentState().edges ) {
		int i = assoc.first;
		// if is neither an explicit failure, nor go-root-failure
		Trie * next = branch(i);
		if ( next == NULL || isRoot(next) )
			continue;
		// if the current (== root) has a fair branch
		branch(i)->fail = &root();
		q.push(branch(i));
	}

	while ( !q.empty() ) {
		setState(*q.front());
		q.pop();

		// skips if == NULL
		for(auto const & assoc : currentState().edges ) {
			int i = assoc.first;
//		for (int i = 0; i < SIGMA_SIZE; i++) {
			Trie * next = branch(i);
//			if (next != NULL && next != &root()) {
			if ( isRoot(*next) )
				continue;
			q.push(next);

			Trie * failnode = currentState().fail;
			while ( branch(*failnode,i) == NULL ) {
				failnode = failnode->fail;
			}
			next->fail = branch(*failnode, i); //->edges.at(i);
			for (const string & str : next->fail->out) {
				next->out.insert(str);
			}
		}
	}
}

// ch is the character for which the machine will make a transition.
void ACTrie::transferState(int ch)  {
	while ( branch(ch) == NULL ) {
		current = current->fail;
	}
	current = branch(ch);
}

void ACTrie::scan(string & text, vector<pair<int,string>> & occurrs) {
	resetState();
//	Trie *node = &root();

	for (int i = 0; i < text.size(); i++) {
		transferState(text[i]);
		if (currentState().out.size() != 0) {
			for (const string & str : currentState().out) {
				occurrs.push_back(pair<int, string>(i-str.length()+1, str));
			}
		}
	}

}

#define NAIVE_PATTERN_ADDITION
#ifdef NAIVE_PATTERN_ADDITION
/*
int addString2(Trie *node, string &curString, int depth = 0, int depth2 = -1) {

	if (depth == curString.size()) {
		node->out.insert(curString);

		return depth2;
	}

	int next = curString[depth] - ' ';

	if (node->edges[next] == NULL || node->edges[next] == root) {
		node->edges[next] = new Trie();
		if (depth2 == -1)
			depth2 = depth;
	}

	addString2(node->edges[next], curString, depth + 1, depth2);
}
 */
void ACTrie::addPattern(const string & patt) {

//	int depth = 0;
//	string curString2;
	Trie *node2 = root;
	Trie *nodepoint;
	Trie *nodepoint2 = root;
	int cur3 = 0;

	//int size2 = 0;
	//size2 = curString2.size();

	//depth = addString2(root, curString2);
	// seems that depth == -1 means no new branch has spawned,
	// i.e. string is existing or is a prefix of some already-added word.

	int extended = 0;
	resetState();
	extended = addString(patt);
//	if (depth == -1) {
	if ( extended == 0 )
//		cout << "depth = -1" << endl;
//		cout << curString2 << endl;
		cout << patt << " is just a prefix of some other pattern." << endl;

		if (count(node2->out.begin(), node2->out.end(), curString2) > 0) {
			;
		}
		else {
			queue<Trie*> q4;
			Trie *rq;

			for (int k = 0; k < size2; k++) {
				cur3 = curString2[k] - ' ';
				node2 = node2->edges[cur3];
			}

			q4.push(node2);

			while (!q4.empty()) {
				rq = q4.front();
				q4.pop();
				rq->out.insert(curString2);
				for (auto s : rq->ine)
					q4.push(s);
			}

		}
	}
	else {

		start = clock();

		if (depth != 0) {
			for (int k = 0; k < depth; k++) {
				cur3 = curString2[k] - ' ';
				node2 = node2->edges[cur3];
				//cout << "cur3 " << cur3 << " " << node2->nodenum << endl;
			}
		}

		nodepoint = node2;
		nodepoint2 = nodepoint;

		Trie *next3 = nodepoint->edges[curString2[depth] - ' '];


		for (int k = depth; k < size2; k++) {
			Trie *next2 = nodepoint->edges[curString2[k] - ' '];

			Trie *f = nodepoint->fail;
			int h = curString2[k] - ' ';

			int p = 0;
			for (; (f->edges[curString2[k] - ' '] == NULL); f = f->fail);

			if (k == 0) {
				next2->fail = root;

				root->ine.push_back(next2);
			}
			else if ((f->edges[curString2[k] - ' '] == root)) {
				next2->fail = root;

				root->ine.push_back(next2);
			}
			else {
				next2->fail = f->edges[curString2[k] - ' '];

				f->edges[curString2[k] - ' ']->ine.push_back(next2);

				if (next2->fail->out.size() > 0)
					for (auto s : next2->fail->out)
						next2->out.insert(s);

			}

			nodepoint = next2;

		}

		end = clock();
		time2 += end - start;



		//既存の関数の更新

		// build the fail function
		queue<Trie*> q2;
		queue<int> q2_num;
		int q2_n = 0;

		start = clock();

		for (int i = 0; i < nodepoint2->ine.size(); i++) {
			if (nodepoint2->ine[i] != next3) {
				q2.push(nodepoint2->ine[i]);
				q2_num.push(depth);
			}
		}

		Trie *temp = root;
		for (int i = 0; i < size2; i++) {
			tr[i] = temp;
			temp = temp->edges[curString2[i] - ' '];

		}


		char curString3[50];

		for (int i = 0; i < size2; i++) {
			curString3[i] = curString2[i];
		}


		curString3[size2] = '*';
		curString3[size2+1] = '\0';

		Trie *r, *rnext;
		Trie *nodepoint3;

		fail_size = 0;
		while (!q2.empty()) {
			r = q2.front();
			q2.pop();
			int i = q2_num.front();
			q2_num.pop();

			nodepoint3 = tr[i];

			while ((r->edges[curString3[i] - ' '] != NULL)) {
				rnext = r->edges[curString3[i] - ' '];

				rnext->fail = nodepoint3->edges[curString3[i] - ' '];
				nodepoint3->edges[curString3[i] - ' ']->ine.push_back(rnext);

				r = rnext;

				nodepoint3 = nodepoint3->edges[curString3[i] - ' '];

				i = i + 1;

			}

			c_start = clock();
			if (size2 <= i) {

				auto itr = r->out.find(curString2);
				if (itr == r->out.end()) {
					r->out.insert(curString2);
					out_size++;
				}
			}
			c_end = clock();
			c_time += c_end - c_start;

			for (int j = 0; j < r->ine.size(); j++) {
				q2.push(r->ine[j]);
				q2_num.push(i);
			}

		}

		end = clock();
		time3 += end - start;

	}
}
#endif

struct WGNode {
	WGNode *edges[SIGMA_SIZE];
	//edge_num[i]  0 = edgeなし 1 = primary-edgeあり 2 = secondary-edgeあり
	int edge_num[SIGMA_SIZE];
	WGNode *suff;
	Trie *dtoc;
	vector<WGNode*> ine;
	//dawgにおけるノードの番号
	int nodenum;
	//ACのノードとdawgのノードの番号を一致させるための変数
	int isTrunk;
	//trunkノードかbranchノードかの判定 1ならtrunk、0ならbranch 初期値は0
	int torb;

	WGNode() : suff(NULL), dtoc(NULL), isTrunk(0), torb(0) {
		nodenum = NODE_NUM_DAWG;
		NODE_NUM_DAWG++;
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
public:
	DAWG() { }

	WGNode & root() { return nroot; }
	WGNode * split(WGNode *parentnode, WGNode *childnode, char a);
	WGNode * update(WGNode *activenode, char a);
	vector<Trie *> getoutstates(string &string);
	vector<Trie *> getfailstates(string &string, int depth);
	void build(ACTrie & actrie, const vector<string>::iterator & start, const vector<string>::iterator & end);
};

WGNode * DAWG::split(WGNode *parentnode, WGNode *childnode, char a) {
	WGNode *newchildnode = new WGNode();
	WGNode *currentnode = parentnode;
	int charnum = a /* - ' ' */;

	parentnode->edges[charnum] = newchildnode;
	parentnode->edge_num[charnum] = 1;

	//childのedgeをすべてnewchildのsecondary-edgeにコピーする
	for (int i = 0; i < SIGMA_SIZE; i++) {
		newchildnode->edges[i] = childnode->edges[i];
		if (childnode->edges[i] != NULL)
			newchildnode->edge_num[i] = 2;
	}

	//parentnode->hassecedgechar[childnode->nodenum] = 0;

	newchildnode->suff = childnode->suff;
	newchildnode->suff->ine.push_back(newchildnode);
	std::vector<WGNode*>::iterator itr =
			std::find(childnode->suff->ine.begin(), childnode->suff->ine.end(), childnode);
	childnode->suff->ine.erase(itr);
	childnode->suff = newchildnode;
	newchildnode->ine.push_back(childnode);

	int m = 0;
	while (currentnode != &nroot) {
		currentnode = currentnode->suff;
		for (m = 0; m < SIGMA_SIZE; m++) {
			if ((currentnode->edges[m] == childnode)
					&& (currentnode->edge_num[m] == 2)) {
				currentnode->edges[m] = newchildnode;
				break;
			}
		}

		if (m == (SIGMA_SIZE - 1))
			break;
	}

	return newchildnode;

}

WGNode * DAWG::update(WGNode *activenode, char a) {

	int charnum = a /* - ' ' */;

	if (activenode->edges[charnum] != NULL) {
		//辺がある場合
		if (activenode->edge_num[charnum] == 1) {
			return activenode->edges[charnum];
		} else {
			return split(activenode, activenode->edges[charnum], a);
		}
	} else {
		//辺がない場合
		WGNode *newactivenode = new WGNode();
		activenode->edges[charnum] = newactivenode;
		activenode->edge_num[charnum] = 1;

		WGNode *currentnode = activenode;

		while ((currentnode != &nroot) && (newactivenode->suff == NULL)) {
			currentnode = currentnode->suff;

			if ((currentnode->edges[charnum] != NULL)
					&& (currentnode->edge_num[charnum] == 1)) {
				newactivenode->suff = currentnode->edges[charnum];
				newactivenode->suff->ine.push_back(newactivenode);
			} else if ((currentnode->edges[charnum] != NULL)
					&& (currentnode->edge_num[charnum] == 2)) {
				newactivenode->suff = split(currentnode,
						currentnode->edges[charnum], a);
				newactivenode->suff->ine.push_back(newactivenode);
			} else {
				currentnode->edges[charnum] = newactivenode;
				currentnode->edge_num[charnum] = 2;
			}
		}

		if (newactivenode->suff == NULL) {
			newactivenode->suff = &nroot;
			nroot.ine.push_back(newactivenode);
		}
		return newactivenode;
	}
}

vector<Trie *> DAWG::getoutstates(string &string) {
	vector<Trie *> outstates;
	WGNode *activenode = &nroot;
	WGNode *node;
	int stringsize = string.size();
	stack<WGNode*> st;

	for (int i = 0; (i < (stringsize)) && (activenode != NULL); i++) {

		activenode = activenode->edges[(int)string[i] /* - ' ' */];

	}

	if (activenode != NULL) {
		queue<WGNode*> queue;
		queue.push(activenode);
		while (!queue.empty()) {
			node = queue.front();
			queue.pop();
			if (node->torb == 1) {
				outstates.push_back(node->dtoc);
			}

			for (unsigned int i = 0; i < node->ine.size(); i++) {
				queue.push(node->ine[i]);
			}

		}
	}

	return (outstates);
}

vector<Trie *> DAWG::getfailstates(string &string, int depth) {
	vector<Trie *> failstates;
	vector<int> tmp;
	WGNode *activenode = &nroot;
	WGNode *node;
	WGNode *enode;
	int num;
	int stringsize = string.size();
	stack<WGNode *> st;
	stack<int> st_num;

	for (int i = 0; (i <= stringsize) && (activenode != NULL); i++) {
		if ((i >= (depth)) && (activenode != NULL)) {
			st.push(activenode);
			st_num.push(i);
		}
		if (i == stringsize)
			break;

		activenode = activenode->edges[(int)string[i] /* - ' ' */];
	}

	queue<WGNode*> queue;
	vector<WGNode*> mark;

	while (!st.empty()) {
		node = st.top();
		st.pop();
		num = st_num.top();
		st_num.pop();
		queue.push(node);

		while (!queue.empty()) {
			enode = queue.front();
			queue.pop();

			auto itr = find(mark.begin(), mark.end(), enode);
			if (itr == mark.end()) {
				mark.push_back(enode);
				if (enode->torb == 1) {
					enode->dtoc->ine_num = num;
					failstates.push_back(enode->dtoc);
				} else {

					for (unsigned int i = 0; i < node->ine.size(); i++) {
						queue.push(node->ine[i]);
					}
				}
			}
		}
	}
	return failstates;
}

void DAWG::build(ACTrie & actrie, const vector<string>::iterator & start, const vector<string>::iterator & end) {
	string wordstr;
	//
	//dawgの構成
	//

	root().isTrunk = DAWGTOAC_NUM;
	DAWGTOAC_NUM++;
	root().torb = 1;

	WGNode *activenode;
	activenode = & root();
	WGNode *tnode;

	Trie *trienode;
	Trie *tmptrie;

	for (std::vector<string>::iterator iter = start; iter != end; ++iter) {
		wordstr = *iter;
		for (int j = 0; j < wordstr.length(); j++)
			activenode = update(activenode, wordstr[j]);

		activenode = &root(); //nroot;
		trienode = &actrie.root();

		activenode = activenode->edges[(int)wordstr[0] /* - ' ' */];
		trienode = trienode->edges[(int)wordstr[0] /* - ' ' */];

		for (int j = 1; j < wordstr.length(); j++) {
			if (activenode->isTrunk == 0) {
				//if (activenode->torb == 1) {
				activenode->isTrunk = DAWGTOAC_NUM;
				activenode->torb = 1;
				DAWGTOAC_NUM++;
				activenode->dtoc = trienode;
			}

			tnode = activenode->edges[(int)wordstr[j] /* - ' ' */];
			tmptrie = trienode->edges[(int)wordstr[j] /* - ' ' */];
			activenode = tnode;
			trienode = tmptrie;
		}
		if (activenode->isTrunk == 0) {
			activenode->isTrunk = DAWGTOAC_NUM;
			activenode->torb = 1;
			DAWGTOAC_NUM++;
			activenode->dtoc = trienode;
		}
		activenode = & root(); //nroot;
	}
}

int getWordsInFile(std::string & filename, vector<string> & words, const int limit_num = 0);

int main(int argc, char * argv[]) {
	std::vector<string> keywords;
	std::istringstream sstream;
	std::string file1name = "word_list2.txt";
	std::string checkfilename = "aho_check_out.txt";

	int word_build_num = 10000;
	int word_add_num = 10000;

	//キーワード（の集合）が入力として与えられる
	//キーワードを用いて、通常の方法でACマシンとDAWGを作る

	//
	//ACマシンの構成
	//

	DAWG dawg;
	ACTrie actrie;

	int out_size = 0;

	//goto関数の構成

	if ( argc == 4 ) {
		file1name = argv[1];
		sstream.str(argv[2]);
		sstream.clear();
		sstream >> word_build_num;
		sstream.str(argv[3]);
		sstream.clear();
		sstream >> word_add_num;
	}

	cout << "Reading file " << file1name << "... ";
	int wcount = getWordsInFile(file1name, keywords);
	if ( wcount < 0 ) {
		std::cerr << std::endl << "failed to open " << file1name << std::endl;
		return EXIT_FAILURE;
	}
	cout << "done. " << std::endl << "Extracted " << wcount << "words. " << std::endl << std::endl;

	actrie.build(keywords.begin(), keywords.begin() + word_build_num);

	cout << "Construct ac machine finished." << endl;

	dawg.build(actrie, keywords.begin(), keywords.begin() + word_build_num);
	cout << "Construct DAWG finished." << endl;
	cout << endl;

	//（動作確認）
	string text;
	std::ifstream ifs(checkfilename);
	if (ifs.fail()) {
		std::cerr << "failed to open " << checkfilename << std::endl;
		return EXIT_FAILURE;
	}
	while ( !ifs.eof() ) {
		string str;
		getline(ifs, str);
		text.append(str);
		text.append("\n");
	}

	cout << "test text = '" << text << "'" << endl;
	cout << "length = " << text.length() << std::endl;
	vector<pair<int,string>> occurrences;
	actrie.scan(text, occurrences);
	for(const pair<int,string> & occurr : occurrences ) {
		std::cout << "'" << occurr.second << "'@" << occurr.first << ", ";
	}
	cout << std::endl << std::endl;
	occurrences.clear();

	//
	//キーワードを新たに与える（一語ずつ）
	//DAWGを動的に構成
	//DAWGをもとにACマシンを更新
	//

	//dynamic start

	cout << "Going to add " << word_add_num << " keywords to the existing machine. " << endl << endl;

		clock_t start, end, total_start, total_end;
		double time1 = 0, time2 = 0, time3 = 0, time4 = 0, time5 = 0;

		total_start = clock();

		for (vector<string>::iterator it = keywords.begin() + word_build_num;
				it != keywords.end() && it != keywords.begin() + word_build_num + word_add_num;
				it++) {
//		for (int i7 = 0; i7 < key_num; i7++) {

			//			Trie *node2 = troot;
//			Trie *nodepoint2 = troot;
			Trie * tactivenode;
			Trie *newstates[50];
//			int depth = 0;

			string curString2 = *it;;
			int size2 = 0;
//			int cur3 = 0;

			WGNode *activenode = &dawg.root(); //nroot;
			WGNode *tnode = &dawg.root(); //nroot;

			//cout << curString2 << " " << i7 << endl;

			size2 = curString2.size();

			//Algorithm1

			tactivenode = &actrie.root();
			int charnum = 0;
			int trie_depth = 10000;

			start = clock();

			for (int i = 0; i < size2; i++) {
				newstates[i] = tactivenode;
				charnum = curString2[i] /* - ' ' */;

				if (tactivenode->edges[charnum] != NULL
						&& tactivenode->edges[charnum] != &actrie.root()) {
					tactivenode = tactivenode->edges[charnum];
				} else {

					if (trie_depth == 10000) {
						trie_depth = i + 1;
					}
					//Trie *newstate = new Trie();
					//tactivenode->edges[charnum] = newstate;
					actrie.addBranch(*tactivenode, charnum);

					tactivenode = tactivenode->edges[charnum];
				}
			}
			newstates[size2] = tactivenode;
			tactivenode->out.insert(curString2);

			end = clock();
			time1 += (end - start);

			start = clock();

			vector<Trie*> failstates;
			failstates = dawg.getfailstates(curString2, trie_depth);
			int fstatesize = failstates.size();

			for (int i = fstatesize - 1; i >= 0; i--) {
				failstates[i]->fail = newstates[failstates[i]->ine_num];
			}

			end = clock();
			time2 += (end - start);

			tactivenode = &actrie.root();
			Trie *failurenode;

			start = clock();

			int charnum2 = 0;

			for (int i = 0; i < size2; i++) {
				charnum2 = curString2[i] /* - ' ' */;

				if ((i + 1) >= trie_depth) {
					if (tactivenode == &actrie.root()) {
						tactivenode->edges[charnum2]->fail = &actrie.root();
						tactivenode = tactivenode->edges[charnum2];
					} else {
						failurenode = tactivenode->fail;

						int j = 0;
						while ((failurenode->edges[charnum2] == NULL)
								|| (failurenode->edges[charnum2] == tactivenode)
								|| (failurenode == &actrie.root())) {

							if (j >= 2)
								break;

							failurenode = failurenode->fail;
							if (failurenode == &actrie.root())
								j++;

						}

						tactivenode = tactivenode->edges[charnum2];

						tactivenode->fail = failurenode->edges[charnum2];

						for (auto s : failurenode->edges[charnum2]->out)
							tactivenode->out.insert(s);

					}

				} else {
					tactivenode = tactivenode->edges[(int)curString2[i] /* - ' ' */];
				}
			}

			end = clock();
			time3 += (end - start);

			vector<Trie *> outstates;

			start = clock();

			outstates = dawg.getoutstates(curString2);

			int outstatesize = outstates.size();

			out_size += outstatesize;

			for (int i = 0; i < outstatesize; i++) {
				outstates[i]->out.insert(curString2);
			}

			end = clock();
			time4 += (end - start);

//			Trie *check_trie = troot;

			activenode = &dawg.root(); //nroot;

			start = clock();

			for (int i = 0; i < size2; i++)
				activenode = dawg.update(activenode, curString2[i]);

			activenode = &dawg.root(); //nroot;
			Trie *trienode = &actrie.root();
			Trie *tmptrie;

			activenode = activenode->edges[(int)curString2[0] /* - ' ' */];
			trienode = trienode->edges[(int)curString2[0] /* - ' ' */];

			for (int i = 1; i < size2; i++) {
				if (activenode->isTrunk == 0) {
					activenode->isTrunk = DAWGTOAC_NUM;
					activenode->torb = 1;
					DAWGTOAC_NUM++;
					activenode->dtoc = trienode;
				}

				tnode = activenode->edges[(int)curString2[i] /* - ' ' */];
				tmptrie = trienode->edges[(int)curString2[i] /* - ' ' */];
				activenode = tnode;
				trienode = tmptrie;
			}

			if (activenode->isTrunk == 0) {
				activenode->isTrunk = DAWGTOAC_NUM;
				activenode->torb = 1;
				DAWGTOAC_NUM++;
				activenode->dtoc = trienode;
			}

			activenode = &dawg.root(); //nroot;

			end = clock();
			time5 += (end - start);

		}

		total_end = clock();
		cout << "total time: "
				<< (double) (total_end - total_start) / CLOCKS_PER_SEC << "sec."
				<< std::endl;
		cout << "time1: " << (double) time1 / CLOCKS_PER_SEC << "sec."
				<< std::endl;
		cout << "time2: " << (double) time2 / CLOCKS_PER_SEC << "sec."
				<< std::endl;
		cout << "time3: " << (double) time3 / CLOCKS_PER_SEC << "sec."
				<< std::endl;
		cout << "time4: " << (double) time4 / CLOCKS_PER_SEC << "sec."
				<< std::endl;
		cout << "time5: " << (double) time5 / CLOCKS_PER_SEC << "sec."
				<< std::endl;

		cout << "out size " << out_size << endl;

		// Read big string, in which we search for elements
		string bigString2;

		getline(ifs, bigString2);
		cout << bigString2 << endl;

		Trie *node3 = &actrie.root();
		int k2 = bigString2.size();
		cout << "string size " << k2 << endl;

		for (int i = 0; i < k2; i++) {

			int cur = bigString2[i] /* - ' ' */;

			for (; node3->edges[cur] == NULL; node3 = node3->fail) {
				cout << "failure now... node number " << node3->fail->nodenum
						<< endl;
			}

			node3 = node3->edges[cur];

			cout << "current node number " << node3->nodenum << endl;

			if (node3->out.size() != 0) {
				cout << "At position " << i << " we found:" << std::endl;

				for (auto s : node3->out) {
					cout << s << std::endl;
				}
			}

		}

	//dynamic finish

	return 0;
}

int getWordsInFile(std::string & filename, vector<string> & words, const int limit_num) {
	std::ifstream file;
	std::istringstream sstream;
	string line, str;
	int count;

	file.open(filename, std::ios::in);
	if ( file.fail() ) {
		return -1;
	}
	count = 0;
	while ( !file.eof() ) {
		if ( limit_num != 0 && !(count < limit_num) )
			break;
		std::getline(file, line);
		sstream.str(line);
		sstream.clear();
		sstream >> str;
		words.push_back(str);
		count++;
	}
	file.close();

	return count;
}

