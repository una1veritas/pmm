#include <iostream>
#include <fstream>
#include <set>
#include <queue>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <time.h>

#include <sstream>

using namespace std;

const int SIGMA_SIZE = 100;
//int NODE_NUM_AHO = 0;
int NODE_NUM_DAWG = 0;
int DAWGTOAC_NUM = 0;

struct Trie {
	Trie *edges[SIGMA_SIZE];
	Trie *fail;
	set<string> out;
	int nodenum;
	int ine_num;

	Trie(const int id) {
		fail = NULL;
		ine_num = 0;
		nodenum = id;
		for (int i = 0; i < SIGMA_SIZE; i++) {
			edges[i] = NULL;
		}

	}
};
//Trie *troot = new Trie();

class ACTrie {
	Trie troot; // = Trie(0);
	int node_num;

public:
	ACTrie() : troot(0), node_num(1) {
		// rootの初期化
		for (int i = 0; i < SIGMA_SIZE; i++) {
			root().edges[i] = &root();
			root().fail = &root();
		}
	}

	unsigned int size() const {
		return node_num;
	}
	Trie & root(void) {
		return troot;
	}
	bool isRoot(const Trie & t) const {
		return &t == &troot;
	}
	void addTransitions(string &str);
	void addState(Trie & t, int ch) {
		t.edges[ch] = new Trie(node_num);
		node_num++;
	}
	void buildMachine(std::vector<std::string> & keywords);
	void scan(string & text);
};

void ACTrie::addTransitions(string & str) {
	Trie * node = &root();
	unsigned int depth = 0;
	int next;

	while (depth < str.size()) {
		next = str[depth] - ' ';
		if (node->edges[next] == NULL || node->edges[next] == &root()) {
			addState(*node, next); //node->edges[next] = new Trie();
		}
		node = node->edges[next];
		depth++;
	}
	node->out.insert(str);
	return;

}

void ACTrie::buildMachine(std::vector<std::string> & words) {
	std::queue<Trie *> q;

	// goto関数と状態の追加
	for(std::vector<string>::iterator it = words.begin(); it != words.end(); it++ ) {
		addTransitions(*it); //Goto(troot, reading_line);
	}

	// failure関数の構成
	// Must to this before, because of the fact that every edge out of the root is
	// not NULL
	for (int i = 0; i < SIGMA_SIZE; i++) {
//		if ((actrie.root().edges[i] != NULL) && (actrie.root().edges[i] != &actrie.root() )) {
		if ( root().edges[i] == NULL || isRoot(*root().edges[i]) )
			continue;
//			std::cout << "actrie.root.edges[" << (char)i << "] is not root." << std::endl;
		root().edges[i]->fail = &root();
		q.push(root().edges[i]);
	}

	while ( !q.empty() ) {
		Trie *curNode = q.front();
		q.pop();

		for (int i = 0; i < SIGMA_SIZE; i++) {
			Trie *next = curNode->edges[i];
//			if (next != NULL && next != &root()) {
			if ( next == NULL || isRoot(*next) )
				continue;

			q.push(next);
			Trie *f = curNode->fail;
			for (; f->edges[i] == NULL; f = f->fail) {}
			next->fail = f->edges[i];
			//for (string s : next->fail->out) {
			next->out.insert(next->fail->out.begin(), next->fail->out.end() );
		}
	}
}

void ACTrie::scan(string & text) {
	Trie *node = &root();
	int k = text.size();

	for (int i = 0; i < k; i++) {
		int cur = text[i] - ' ';

		for (; node->edges[cur] == NULL; node = node->fail) {
			cout << "failure now... node number " << node->fail->nodenum
					<< endl;
		}

		node = node->edges[cur];

		cout << "current node number " << node->nodenum << endl;

		if (node->out.size() != 0) {
			cout << "At position " << i << " we found:" << std::endl;

			for (string s : node->out) {
				cout << s << std::endl;
			}
		}
	}

}


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

	WGNode() {
		suff = NULL;
		dtoc = NULL;
		torb = 0;
		isTrunk = 0;
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
};

WGNode * DAWG::split(WGNode *parentnode, WGNode *childnode, char a) {
	WGNode *newchildnode = new WGNode();
	WGNode *currentnode = parentnode;
	int charnum = a - ' ';

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

	int charnum = a - ' ';

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

		activenode = activenode->edges[string[i] - ' '];

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

		activenode = activenode->edges[string[i] - ' '];
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

int main(int argc, char * argv[]) {

	std::istringstream sstream;

	//キーワード（の集合）が入力として与えられる
	//キーワードを用いて、通常の方法でACマシンとDAWGを作る

	//
	//ACマシンの構成
	//

	DAWG dawg;
	ACTrie actrie;
#ifdef USE_ORIGINAL
	// rootの初期化
	for (int i = 0; i < SIGMA_SIZE; i++) {
		actrie.root().edges[i] = &actrie.root();
		actrie.root().fail = &actrie.root();
	}
#endif
	int out_size = 0;

	//goto関数の構成

	std::ifstream reading_file;
	std::string file1name;
	std::string checkfilename;

	if (argc > 1)
		file1name = argv[1];
	else
		file1name = "word_list2.txt";
	//file_name = "word_2000.txt";
	//file_name = "test5.txt";
	//file_name = "word_list3.txt";
	//file_name = "word_list2.txt";
	//file_name = "aho_check_in.txt";

	int word_num;
	string wordstr;
//		string ward_num;
//		int ward_num_i;
//		string ward_string;

	string reading_line;
	std::vector<std::string> keywords;

	reading_file.open(file1name, std::ios::in);
	if (reading_file.fail()) {
		std::cerr << "failed to open " << file1name << std::endl;
		return -1;
	}
	std::getline(reading_file, reading_line);
	sstream.str(reading_line);
	sstream.clear();
//		ward_num = reading_line;
//		ward_num_i = atoi(ward_num.c_str());
	sstream >> word_num;
	for (int i = 0; i < word_num; i++) {
		std::getline(reading_file, reading_line);
		sstream.str(reading_line);
		sstream.clear();
		sstream >> wordstr;
		//cout << reading_line << " " << i << endl;
		std::cout << i << ": '" << wordstr << "'" << std::endl;
		keywords.push_back(wordstr);
	}

#ifdef USE_ORIGINAL
	for(std::vector<string>::iterator it = keywords.begin(); it != keywords.end(); it++ )
		actrie.addTransitions(*it); //Goto(troot, reading_line);

	// failure関数の構成
	queue<Trie*> q;

	// Must to this before, because of the fact that every edge out of the root is
	// not NULL
	for (int i = 0; i < SIGMA_SIZE; i++) {
//		if ((actrie.root().edges[i] != NULL) && (actrie.root().edges[i] != &actrie.root() )) {
		if ((actrie.root().edges[i] != NULL)
				&& !actrie.isRoot(*actrie.root().edges[i])) {
			std::cout << "actrie.root.edges[" << (char)i << "] is not root." << std::endl;
			actrie.root().edges[i]->fail = &actrie.root();
			q.push(actrie.root().edges[i]);
		}
	}

	//Trie *tnode;
	//cout << "check point1" << endl;

//	int j = 1;
	while (!q.empty()) {
		Trie *curNode = q.front();
		q.pop();

		for (int i = 0; i < SIGMA_SIZE; i++) {
			Trie *next = curNode->edges[i];
			if (next != NULL && next != &actrie.root()) {
				q.push(next);

				Trie *f = curNode->fail;
				for (; f->edges[i] == NULL; f = f->fail)
					;

				next->fail = f->edges[i];

				for (auto s : next->fail->out) {
					next->out.insert(s);
				}
			}
		}
	}
#else
	actrie.buildMachine(keywords);
#endif
	cout << "aho struct finish" << endl;

	//
	//dawgの構成
	//

	dawg.root().isTrunk = DAWGTOAC_NUM;
	DAWGTOAC_NUM++;
	dawg.root().torb = 1;

	//string ward_num;
	//int ward_num_i;
	//string ward_string;
	//string file_name;

	WGNode *activenode;
	activenode = & dawg.root();
	WGNode *tnode;

	Trie *trienode;
	Trie *tmptrie;

	//cout << "ファイル名を入力してください" << endl;
	//cin >> file_name;

	//file_name = "word_2000.txt";
	//file_name = "test5.txt";
	//file_name = "word_list3.txt";
	//file_name = "word_list2.txt";
	//file_name = "aho_check_in.txt";

	std::ifstream reading_file2;
	//string reading_line;

	reading_file2.open(file1name, std::ios::in);
	if (reading_file2.fail()) {
		std::cerr << "failed to open " << file1name << std::endl;
		return -1;
	}
	std::getline(reading_file2, reading_line);
	sstream.str(reading_line);
	sstream.clear();
	//ward_num = reading_line;
	//ward_num_i = atoi(ward_num.c_str());
	sstream >> word_num;

	for (int i = 0; i < word_num; i++) {

		std::getline(reading_file2, reading_line);

		//cout << reading_line << " " << i << endl;

		int stringsize = reading_line.size();

		for (int j = 0; j < stringsize; j++)
			activenode = dawg.update(activenode, reading_line[j]);

		activenode = &dawg.root(); //nroot;
		trienode = &actrie.root();

		activenode = activenode->edges[reading_line[0] - ' '];
		trienode = trienode->edges[reading_line[0] - ' '];

		for (int j = 1; j < stringsize; j++) {
			if (activenode->isTrunk == 0) {
				//if (activenode->torb == 1) {
				activenode->isTrunk = DAWGTOAC_NUM;
				activenode->torb = 1;
				DAWGTOAC_NUM++;
				activenode->dtoc = trienode;
			}

			tnode = activenode->edges[reading_line[j] - ' '];
			tmptrie = trienode->edges[reading_line[j] - ' '];
			activenode = tnode;
			trienode = tmptrie;
		}
		if (activenode->isTrunk == 0) {
			activenode->isTrunk = DAWGTOAC_NUM;
			activenode->torb = 1;
			DAWGTOAC_NUM++;
			activenode->dtoc = trienode;
		}

		activenode = & dawg.root(); //nroot;
	}

	cout << "dawg struct finish" << endl;

	//（動作確認）

	// テキストからキーワードを検出
	string bigString;
	//std::ifstream ifs("test4.txt");
	checkfilename = "aho_check_out.txt";
	std::ifstream ifs(checkfilename);
	if (ifs.fail()) {
		std::cerr << "failed to open " << checkfilename << std::endl;
		return -1;
	}
	getline(ifs, bigString);
	cout << bigString << endl;
#ifdef USE_ORIGINAL
	Trie *node = &actrie.root();
	int k = bigString.size();
	for (int i = 0; i < k; i++) {
		int cur = bigString[i] - ' ';

		for (; node->edges[cur] == NULL; node = node->fail) {
			cout << "failure now... node number " << node->fail->nodenum
					<< endl;
		}

		node = node->edges[cur];

		cout << "current node number " << node->nodenum << endl;

		if (node->out.size() != 0) {
			cout << "At position " << i << " we found:" << std::endl;

			for (string s : node->out) {
				cout << s << std::endl;
			}
		}
	}
#else
	actrie.scan(bigString);
#endif

	//
	//キーワードを新たに与える（一語ずつ）
	//DAWGを動的に構成
	//DAWGをもとにACマシンを更新
	//

	//std::ifstream readline2("word_list3.txt");
	//std::ifstream readline2("test3.txt");
	std::ifstream readline2("aho_check_in_plus.txt");

	//dynamic start

	while (1) {

		int key_num = 0;

		cout << "How many keywords would you like to add?" << endl;
		cin >> key_num;

		//std::ifstream readline2("word_list3.txt");
		//std::ifstream readline2("test3.txt");
		//std::ifstream readline2("aho_check_in_plus.txt");

		clock_t start, end, total_start, total_end;
		double time1 = 0, time2 = 0, time3 = 0, time4 = 0, time5 = 0;

		total_start = clock();

		for (int i7 = 0; i7 < key_num; i7++) {
//			Trie *node2 = troot;
//			Trie *nodepoint2 = troot;
			Trie * tactivenode;
			Trie *newstates[50];
//			int depth = 0;

			string curString2;
			int size2 = 0;
//			int cur3 = 0;

			WGNode *activenode = &dawg.root(); //nroot;
			WGNode *tnode = &dawg.root(); //nroot;

			//キーワードの読み込み

			//getline(readline2, curString2);
			getline(reading_file, curString2);

			//cout << curString2 << " " << i7 << endl;

			size2 = curString2.size();

			//Algorithm1

			tactivenode = &actrie.root();
			int charnum = 0;
			int trie_depth = 10000;

			start = clock();

			for (int i = 0; i < size2; i++) {
				newstates[i] = tactivenode;
				charnum = curString2[i] - ' ';

				if (tactivenode->edges[charnum] != NULL
						&& tactivenode->edges[charnum] != &actrie.root()) {
					tactivenode = tactivenode->edges[charnum];
				} else {

					if (trie_depth == 10000) {
						trie_depth = i + 1;
					}

					//Trie *newstate = new Trie();
					//tactivenode->edges[charnum] = newstate;
					actrie.addState(*tactivenode, charnum);

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
				charnum2 = curString2[i] - ' ';

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
					tactivenode = tactivenode->edges[curString2[i] - ' '];
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

			activenode = activenode->edges[curString2[0] - ' '];
			trienode = trienode->edges[curString2[0] - ' '];

			for (int i = 1; i < size2; i++) {
				if (activenode->isTrunk == 0) {
					activenode->isTrunk = DAWGTOAC_NUM;
					activenode->torb = 1;
					DAWGTOAC_NUM++;
					activenode->dtoc = trienode;
				}

				tnode = activenode->edges[curString2[i] - ' '];
				tmptrie = trienode->edges[curString2[i] - ' '];
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

			int cur = bigString2[i] - ' ';

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

	}
	//dynamic finish

	return 0;
}
