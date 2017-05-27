#include "MinDFA.h"
#include <string>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>

using namespace std;

MinDFA::MinDFA(string initial, vector<string> finals, vector<vector<string> > transitions){
	_initialState = initial;
	_finalStates = finals;
	_transitionFunctions = transitions;
}

string MinDFA::getMinDFAInitialState(){
	return this->_minInitialState;
}

vector<string> MinDFA::getMinDFAFinalStates(){
	return this->_minFinalStates;
}

vector<vector<string> > MinDFA::getMinDFATransitionFunctions(){
	return this->_minTransitionFunctions;
}

void MinDFA::findStates(){

	this->_states.push_back(this->_initialState);
	this->_states.insert(this->_states.end(), this->_finalStates.begin(), this->_finalStates.end());

	vector<vector<string> >::const_iterator row;
	for (row = this->_transitionFunctions.begin(); row != this->_transitionFunctions.end(); ++row){

		if((find(this->_finalStates.begin(), this->_finalStates.end(), *row->begin()) == this->_finalStates.end())
				&& (*row->begin() != this->_initialState)
				&& (find(this->_states.begin(), this->_states.end(), *row->begin()) == this->_states.end())){
			this->_states.push_back(*row->begin());
		}
		else if((find(this->_finalStates.begin(), this->_finalStates.end(), *row->rbegin()) == this->_finalStates.end()) &&
				(*row->rbegin() != this->_initialState)
				&& (find(this->_states.begin(), this->_states.end(), *row->rbegin()) == this->_states.end())){
			this->_states.push_back(*row->rbegin());
		}
	}
}

void MinDFA::findTerminals(){

	for(int i = 0; i != this->_transitionFunctions.size(); i++)
		if(find(this->_terminals.begin(), this->_terminals.end(), this->_transitionFunctions[i][1]) == this->_terminals.end())
			this->_terminals.push_back(this->_transitionFunctions[i][1]);
}

void MinDFA::deleteNoneReachableStates(){

	vector<int> index;

	for(int i = 1; i != this->_states.size(); i++){
		bool flag = true;
		for(int j = 0; j != this->_transitionFunctions.size(); j++){
			if((this->_states[i] == this->_transitionFunctions[j][2]) && (this->_transitionFunctions[j][0] != this->_states[i])){
				flag = false;
				break;
			}
		}
		if(flag){
			index.push_back(i);
		}
	}

	for(int i = 0; i < index.size(); i++)
		this->_states.erase(this->_states.begin() + index[i]);

}

int MinDFA::findElementIndex(vector<string> v, string el){
	bool flag = true;
	int i;
	for(i = 0; i != v.size(); i++){
		if(v[i] == el){
			flag = false;
			break;
		}
	}
	if(flag) return -1;
	return i;
}

template<typename Out>
void MinDFA::split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

vector<string> MinDFA::split(const string &s, char delim) {
	vector<std::string> elems;
	this->split(s, delim, std::back_inserter(elems));
	return elems;
}


void MinDFA::makeMinimalDfa(){

	this->findStates();
	this->findTerminals();
	this->deleteNoneReachableStates();
	int tableSize = this->_states.size() - 1;
	struct tableBlock table[tableSize][tableSize];
	vector<pair<string, string>> state_pairs;

	//finding dependencies
	for(int i = 0; i != tableSize; i++){
		for(int j = 0; j != tableSize; j++){
			if( i + j <= i + i){
				//distinguishable = true for final and none-final
				if((find(this->_finalStates.begin(), this->_finalStates.end(), this->_states[i+1]) != this->_finalStates.end()) &&
					(find(this->_finalStates.begin(), this->_finalStates.end(), this->_states[j]) == this->_finalStates.end())){
					table[i][j].distinguishable = true;
					continue;
				}
				else if((find(this->_finalStates.begin(), this->_finalStates.end(), this->_states[j]) != this->_finalStates.end()) &&
					(find(this->_finalStates.begin(), this->_finalStates.end(), this->_states[i+1]) == this->_finalStates.end())){
					table[i][j].distinguishable = true;
					continue;
				}
				//both states are either final or none-final
				else{
					for(int y = 0; y != this->_terminals.size(); y++){
						string tmp1, tmp2;
						for(int k = 0; k != this->_transitionFunctions.size(); k++){
							if(((this->_transitionFunctions[k][0] == this->_states[i+1]) && (this->_transitionFunctions[k][1] == this->_terminals[y]))){
								tmp1 = this->_transitionFunctions[k][2];
							}
							if((this->_transitionFunctions[k][0] == this->_states[j]) && (this->_transitionFunctions[k][1] == this->_terminals[y])){
								tmp2 = this->_transitionFunctions[k][2];
							}
						}
						if(tmp1 != tmp2){
							if((find(this->_finalStates.begin(), this->_finalStates.end(), tmp1) != this->_finalStates.end()) &&
								(find(this->_finalStates.begin(), this->_finalStates.end(), tmp2) == this->_finalStates.end())){
								table[i][j].distinguishable = true;
								break;
							}
							else if((find(this->_finalStates.begin(), this->_finalStates.end(), tmp2) != this->_finalStates.end()) &&
								(find(this->_finalStates.begin(), this->_finalStates.end(), tmp1) == this->_finalStates.end())){
								table[i][j].distinguishable = true;
								break;
							}
							table[i][j].relations.push_back(tmp1 + " " + tmp2);
						}
					}
				}
			}
		}
	}

	//resolving dependencies
	for(int i = 0; i != tableSize; i++){
			for(int j = 0; j != tableSize; j++){
				if( i + j <= i + i){
					if(!table[i][j].distinguishable && (table[i][j].relations.size() != 0)){
						for(int k = 0; k != table[i][j].relations.size(); k++){
							vector<string> temp = this->split(table[i][j].relations[k], ' ');
							int iIndex, jIndex;
							if(this->findElementIndex(this->_states, temp[0]) >= 1){
								iIndex = this->findElementIndex(this->_states, temp[0]) - 1;
								jIndex = this->findElementIndex(this->_states, temp[1]);
							}else{
								iIndex = this->findElementIndex(this->_states, temp[1]) -1;
								jIndex = this->findElementIndex(this->_states, temp[0]);
							}
							if(table[iIndex][jIndex].distinguishable){
								table[i][j].distinguishable = true;
								break;
							}
							if(!table[iIndex][jIndex].distinguishable && (table[iIndex][jIndex].relations.size() == 0)){
								continue;
							}
						}
					}
					if(!table[i][j].distinguishable){
						state_pairs.push_back(make_pair(this->_states[i+1], this->_states[j]));
					}
				}
			}
	}


	//min dfa initial state
	this->_minInitialState = this->_initialState;
	for(int i = 0; i != state_pairs.size(); i++){
		if(state_pairs[i].first == this->_initialState){
			this->_minInitialState = this->_minInitialState + state_pairs[i].second;
		}
		else if(state_pairs[i].second == this->_initialState){
			this->_minInitialState = this->_minInitialState + state_pairs[i].first;
		}
	}

	//min dfa final states
	for(int i = 0; i != this->_finalStates.size(); i++){
		if(find(this->_minFinalStates.begin(), this->_minFinalStates.end(), this->_finalStates[i]) == this->_minFinalStates.end()){
			bool flag = true;
			for(int j = 0; j != this->_minFinalStates.size(); j++){
				if(this->_minFinalStates[j].find(this->_finalStates[i]) != string::npos){
					flag = false;
					break;
				}
			}
			if(flag){
				string tmp = this->_finalStates[i];
				for(int j = 0; j != state_pairs.size(); j++){
					if(state_pairs[j].first == this->_finalStates[i]){
						tmp += state_pairs[j].second;
					}
					else if(state_pairs[j].second == this->_finalStates[i]){
						tmp += state_pairs[j].first;
					}
				}
				this->_minFinalStates.push_back(tmp);
			}
		}
	}

	//min dfa states
	this->_minStates.push_back(this->_minInitialState);
	this->_minStates.insert(this->_minStates.end(), this->_minFinalStates.begin(), this->_minFinalStates.end());
	for(int i = 0; i != this->_states.size(); i++){
		if(find(this->_minStates.begin(), this->_minStates.end(), this->_states[i]) == this->_minStates.end()){
			bool flag = true;
			for(int j = 0; j != this->_minStates.size(); j++){
				if(this->_minStates[j].find(this->_states[i]) != string::npos){
					flag = false;
					break;
				}
			}
			if(flag){
				string tmp = this->_states[i];
				for(int j = 0; j != state_pairs.size(); j++){
					if(state_pairs[j].first == this->_states[i]){
						tmp += state_pairs[j].second;
					}
					else if(state_pairs[j].second == this->_states[i]){
						tmp += state_pairs[j].first;
					}
				}
			}
		}
	}



	//test
	cout << endl << endl << "States:  " << endl;
	for(int i = 0; i != this->_states.size(); i++)
		cout << this->_states[i] << "\t";

	cout << endl << endl << "TableBlocks: " << endl;
	for(int i = 0; i != tableSize; i++){
		for(int j = 0; j != tableSize; j++){
			if(i + j <= i + i){
				cout << noboolalpha << table[i][j].distinguishable << " ";
			}
		}
		cout << endl;
	}

	cout << endl << "Pairs: " << endl;
	for(int i = 0; i != state_pairs.size(); i++){
		cout << state_pairs[i].first << " \t " << state_pairs[i].second << endl;
	}

	//test
	cout << endl << endl << "Initial State: "<< this->_minInitialState << endl << endl;
	cout << "Final State: " << endl;
	for(int i = 0; i != this->_minFinalStates.size() ; i++)
		cout << this->_minFinalStates[i] << "\t" ;
	cout << endl;

	cout << endl << endl << "Min States: " << endl;
	for(int i = 0 ; i != this->_minStates.size(); i++)
		cout << this->_minStates[i] << "\t";
	cout << endl;

}
