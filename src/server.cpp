#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>
#include <fstream>
#include <unordered_map>
#include <vector>

#include "slanglib.h"

using namespace std;

vector<string> stringVector;

void readIntoMap(unordered_map<string, bool>& stringHash) {
	ifstream inFile;
	string fileName = "words.txt";
	inFile.open(fileName);
	string word;
	int count = 0;
	while(inFile >> word) {
		stringHash[word] = 1;
	}
	inFile.close();
}

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

bool isValidWord(string word, unordered_map<string, bool>& stringHash) {
	return stringHash.find(word) != stringHash.end();
}

int randNumGen(int maxInt) {
	srand(time(NULL));
	return rand() % maxInt;
}

void *accepted(void *arg) {
	int sock = *(int *)arg;
	SlangWrite(sock, "5(HELLO)");
	char *word = SlangRead(sock, NULL);
	free(word);
	
	
	int randIndex = randNumGen(stringVector.size());
	const string correctAns = stringVector[randIndex];
	cout << "RANDOMLY GENERATED CORRECT ANSWER: " << correctAns << endl;
	
	
	SlangWrite(sock, "5(START)");
	close(sock);
	return NULL;
}


int main(int argc, char **argv) {
	int portNumber = 0;
	if(argc > 1) {
		portNumber = atoi(argv[1]);
		if (portNumber < 1024 || portNumber > 65535) {
			printf("%s [port]", argv[0]);
			return EXIT_SUCCESS;
		}
	}
	unordered_map<string, bool> stringHash;
	readIntoMap(stringHash);
	readIntoVector(stringVector);
	SlangLib slang(portNumber, &accepted);
	return 0;
}