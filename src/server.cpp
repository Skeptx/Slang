#include <cstdio>
#include <cstdlib>
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
        inFile.open(fileName);


        string word;
        while(inFile >> word) {


                stringVector.push_back(word);
        }
        inFile.close();
}


int randomNumGen(int maxInt) {


        srand(time(NULL));
        return rand() % maxInt;
}


int main(int argc, char** argv) {


	//unordered_map<string, bool> stringHash;
	//readIntoMap(stringHash);
	const string hostName = "acad.kutztown.edu";
	if(argc < 2) {


                cout << "./slang-server <port>" << endl;
                exit(EXIT_FAILURE);
	}
	const int portNumber = atoi(argv[1]);


        vector<string> stringVector;
        readIntoVector(stringVector);
        int randIndex = randomNumGen(stringVector.size());


        const string correctAns = stringVector[randIndex];
        cout << "RANDOMLY GENERATED CORRECT ANSWER: " << correctAns << endl;


	SlangLib slang('S', portNumber, hostName);
        slang.init();


	return 0;
}
