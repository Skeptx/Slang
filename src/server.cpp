#include <cstdio>
#include <iostream>
#include <random>
#include <fstream>
#include <vector>
#include "libslang.h"
using namespace std;

/*
void readIntoMap(unordered_map<string, bool>& stringHash) {


        ifstream inFile;
        string fileName = "words.txt";
        inFile.open(fileName);


        string word;
        while(inFile >> word) {


                stringHash[word] = 1;
        }
        inFile.close();


}
*/

void readIntoVector(vector<string>& stringVector) {


        ifstream inFile;
        string fileName = "words.txt";


        string word;
        while(inFile >> word) {


                stringVector.push_back(word);
        }
}


int randomNumGen(int maxInt) {


        default_random_engine gen;
        uniform_int_distribution<int> dist(0, maxInt - 1);
        return dist(gen);
}

int main() {


	//unordered_map<string, bool> stringHash;
	//readIntoMap(stringHash);
        vector<string> stringVector;
        readIntoVector(stringVector);


        const string correctAns = stringVector[randomNumGen(stringVector.size())];
        cout << "RANDOMLY GENERATED CORRECT ANSWER: " << correctAns << endl;

	SlangLib slang('S', 46257, "acad.kutztown.edu");
        slang.init();


	return 0;
}
