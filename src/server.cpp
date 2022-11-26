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
        inFile.close();


}


int main() {


	unordered_map<string, bool> stringHash;
	readIntoMap(stringHash);


	SlangLib slang('S', 46257, "acad.kutztown.edu");
	cout << "Test 1: " << slang.checkWord("NOOSL","POOLS");
	cout << "Test 2: " << slang.checkWord("POOLS", "POOLS");
	cout << "Test 3: " << slang.checkWord("NKHOO", "POOLS");
        //slang.init();


	return 0;
}
