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

int readIntoVector(vector<string>& stringVector) {


        ifstream inFile;
        string fileName = "words.txt";


        string word;
        int length = 0;
        while(inFile >> word) {


                stringVector.push_back(word);
                length++;
        }


        return length;
}


int randomNumGen(int maxInt) {


        default_random_engine gen;
        uniform_int_distribution<int> dist(1, maxInt);
        return dist(gen);
}

int main() {


	//unordered_map<string, bool> stringHash;
	//readIntoMap(stringHash);
        vector<string> stringVector;
        int fileLength = readIntoVector(stringVector);


        const string correctAns = stringVector[randomNumGen(fileLength)];
        cout << "RANDOMLY GENERATED CORRECT ANSWER: " << correctAns << endl;

	SlangLib slang('S', 46257, "acad.kutztown.edu");
        slang.init();


	return 0;
}
