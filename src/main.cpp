#include <iostream>
#include "MinDFA.h"
#include <string>
#include <vector>

using namespace std;

int main() {

	string init = "a";
	vector<string> final;
	final.push_back("d");
	final.push_back("c");
	final.push_back("e");

	vector<vector<string> > tran;

	tran.push_back(vector<string>{"a", "0", "b"});
	tran.push_back(vector<string>{"a", "1", "c"});
	tran.push_back(vector<string>{"b", "0", "a"});
	tran.push_back(vector<string>{"b", "1", "d"});
	tran.push_back(vector<string>{"c", "0", "e"});
	tran.push_back(vector<string>{"c", "1", "f"});
	tran.push_back(vector<string>{"d", "0", "e"});
	tran.push_back(vector<string>{"d", "1", "f"});
	tran.push_back(vector<string>{"e", "0", "e"});
	tran.push_back(vector<string>{"e", "1", "f"});
	tran.push_back(vector<string>{"f", "0", "f"});
	tran.push_back(vector<string>{"f", "1", "f"});


	MinDFA mindfa = MinDFA(init, final, tran);
	mindfa.makeMinimalDfa();


	return 0;
}
