#include <cstdio>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "libslang.h"
using namespace std;


void readIntoMap(unordered_map<string, bool>& stringHash) {


        ifstream inFile;
        string fileName = "words.txt";
        inFile.open(fileName);


        string word;
        while(inFile >> word) {


                stringHash[word] = 1;
        }
}

void messageHandler(char *message) {
}

int main() {


	unordered_map<string, bool> stringHash;
	readIntoMap(stringHash);


	string test;
        cout << "Map Search: " << flush;
        cin >> test;
        if(stringHash.find(test) == stringHash.end()) {


                cout << "Word Not Found!" << endl;
        }
        else {


                cout << "Word Found! WOO!" << endl;
        }


	SlangLib slang('S', 46257, "acad.kutztown.edu");


	return 0;
}
