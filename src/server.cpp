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
	if(argc < 3) {


                cout << "./slang-server <hostname> <port>" << endl;
                exit(EXIT_FAILURE);
	}
	const string hostName = argv[1];
	const int portNumber = atoi(argv[2]);


        vector<string> stringVector;
        readIntoVector(stringVector);
        int randIndex = randomNumGen(stringVector.size());


        const string correctAns = stringVector[randIndex];
        cout << "RANDOMLY GENERATED CORRECT ANSWER: " << correctAns << endl;


	SlangLib slang('S', portNumber, hostName);
        slang.init();


	return 0;
}
