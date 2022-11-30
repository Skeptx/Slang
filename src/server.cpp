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
	int guesses;
	while (!strcmp(buffer, "5(READY)")) {


		int randIndex = randNumGen(stringVector.size());
		const string correctAns = stringVector[randIndex];
		SlangWrite(sock, "5(START)");
		string guessWord;
		string response = "5(START)";
		guesses = 1;
		while(guesses <= 6) {


			SlangRead(sock, buffer);
			if(!strcmp(buffer, "4(QUIT)")) {


				free(buffer);
				close(sock);
				return NULL;
			}
			else if(!strcmp(buffer, "5(REPLY)")) {


				SlangWrite(sock, response.c_str());
			}
			else {


				guessWord = "";
				for(int i = 2; i <= 6; i++) {


					guessWord += buffer[i];
				}
				if(isValidWord(guessWord)) {


					guessWord = SlangCheck(guessWord, correctAns);
					response = "R(" + guessWord + ")";
					if (strcmp(guessWord.c_str(), "44444")) {
						if (guesses == 6) {
							response = "A(" + guessWord + correctAns + ")";
						}
						++guesses;
					} else {
						guesses = 8;
					}
				}
				else {
					response = "5(WRONG)";
				}
				SlangWrite(sock, response.c_str());
			}
		}
		SlangRead(sock, buffer);
	}
	int err = pthread_mutex_lock(&m);
	if (err) {
		fprintf(stderr, "Error: pthread_mutex_lock failed: %s\r\n", strerror(err));
	}
	free(buffer);
	close(sock);
	vector<int>::iterator itSocket = find(vecOfSockets.begin(), vecOfSockets.end(), sock);
	vector<pthread_t>::iterator itThread = find(vecOfThreads.begin(), vecOfThreads.end(), pthread_self());
	if (itSocket != vecOfSockets.end()) {
		vecOfSockets.erase(itSocket);
	}
	if (itThread != vecOfThreads.end()) {
		vecOfThreads.erase(itThread);
	}
	err = pthread_mutex_unlock(&m);
	if (err) {
		fprintf(stderr, "Error: pthread_mutex_unlock failed: %s\r\n", strerror(err));
	}
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