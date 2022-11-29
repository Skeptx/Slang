#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
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
	char *buffer = SlangRead(sock, NULL);
	int guesses = 1;
	while (!strcmp(buffer, "5(READY)")) {


		int randIndex = randNumGen(stringVector.size());
		const string correctAns = stringVector[randIndex];
		SlangWrite(sock, "5(START)");


                while(guesses <= 6) {


                        buffer = SlangRead(sock, buffer);
                        if(!strcmp(buffer, "4(QUIT)") && !strcmp(buffer, "5(REPLY)")) {


                                string guessWord;
                                for(int i = 2; i <= 6; i++) {


                                        guessWord += buffer[i];
                                }
                                if(isValidWord(guessWord)) {


                                        guessWord = SlangCheck(guessWord, correctAns);
                                        guessWord = "R(" + guessWord + ")";
                                        SlangWrite(sock, guessWord.c_str());
                                        guesses++;
                                }
                                else {
                                        SlangWrite(sock, "5(WRONG)");
                                }
                        }
                        // MORE READS AND WRITES
                        // KEEP TRACK OF NUMBER OF GUESSES
                }
	}
	free(buffer);
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
