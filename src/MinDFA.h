#include <string>
#include <vector>

using namespace std;

#ifndef MINDFA
#define MINDFA

struct tableBlock{
	bool distinguishable = false;
	vector<string> relations;
};


class MinDFA{
private:
	string _initialState;
	vector<string> _finalStates;
	vector<vector<string> > _transitionFunctions;
	vector<string> _states;
	vector<string> _terminals;

	string _minInitialState;
	vector<string> _minFinalStates;
	vector<vector<string> > _minTransitionFunctions;
	vector<string> _minStates;


	void findStates();
	void findTerminals();
	void deleteNoneReachableStates();
	int findElementIndex(vector<string> v, string el);
	vector<string> split(const string &s, char delim);
	template<typename Out>
	void split(const std::string &s, char delim, Out result);

public:
	MinDFA(string initial, vector<string> final, vector<vector<string> > transition);
	void makeMinimalDfa();
	string getMinDFAInitialState();
	vector<string> getMinDFAFinalStates();
	vector<vector<string> > getMinDFATransitionFunctions();

};

#endif
