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
unordered_map<string, bool> stringHash;

void readIntoMap() {
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


void readIntoVector() {
	ifstream inFile;
	string fileName = "words.txt";
	inFile.open(fileName);
	string word;
	while(inFile >> word) {
		stringVector.push_back(word);
	}
	inFile.close();
}


bool isValidWord(string word) {
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


                string guessWord;
                string response;
                while(guesses <= 6) {


                        buffer = SlangRead(sock, buffer);
                        if(strcmp(buffer, "4(QUIT)") && strcmp(buffer, "5(REPLY)")) {


                                guessWord = "";
                                response = "";
                                for(int i = 2; i <= 6; i++) {


                                        guessWord += buffer[i];
                                }
                                if(isValidWord(guessWord)) {


                                        guessWord = SlangCheck(guessWord, correctAns);
                                        response = "R(" + guessWord + ")";
                                        SlangWrite(sock, response.c_str());
                                        guesses++;
                                }
                                else {

                                        response = "5(WRONG)";
                                        SlangWrite(sock, response.c_str());
                                }
                        }
                        else if(!strcmp(buffer, "4(QUIT)")) {


                                cout << "Quitting Server..." << endl;
                                free(buffer);
                                close(sock);
                                return NULL;
                        }
                        else if(!strcmp(buffer, "5(REPLY)")) {


                                SlangWrite(sock, response);
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
	readIntoMap();
	readIntoVector();
	SlangLib slang(portNumber, &accepted);
	return 0;
}
