// Author: Patrick Perrin (with help from Henry Morales and Isaiah Rovenolt)
// Major: Computer Science
// Creation Date: 10/24/2022
// Due Date: 11/29/2022
// Course: CSC328
// Professor: Dr. Frye
// Assignment: NWProgram - Wordle
// File Name: server.cpp
// Purpose: This is the server application codebase for a version of Wordle
//      called "Slang". It utilizes the Slang library, SlangLib, to create a
//      SlangLib object which contains info pertinent to the server as well as
//      clients.
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <fstream>
#include <unordered_map> // Necessary only for constant time search
#include <vector> // Necessary for subscripting on random element

#include "slanglib.h"

using namespace std;

vector<string> stringVector;
unordered_map<string, bool> stringHash;


// Function name: readIntoMap
// Description: Reads words.txt file which contains valid 5-letter words into
//      a hash map structure which is used for constant time searches
// Parameters: none
// Return Value: void - none
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


// Function name: readIntoVector
// Description: Reads words.txt file which contains valid 5-letter words into
//      a vector structure for subscripting on a random index for word selection
// Parameters: none
// Return Value: void - none
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


// Function name: isValidWord
// Description: Validates the legitimacy of a word based on whether it is found
//      in the stringHash hashmap
// Parameters: string word - word to validate
// Return Value: void - none
bool isValidWord(string word) {
	return stringHash.find(word) != stringHash.end();
}


// Function name: randNumGen
// Description: Seeds and generates a random number to be used as the index
//      in the selection of a random word
// Parameters: int maxInt - the upper bound of the PRNG
// Return Value: int - returns (pseudo)randomly generated number
int randNumGen(int maxInt) {
	srand(time(NULL));
	return rand() % maxInt;
}


// Function name: accepted
// Description: Handles the functionality of the server application when a
//      client connects. This function is passed to the library which handles
//      the creation of the individual threads per client connection.
// Parameters: void* arg - the socket to be used
// Return Value: void* - none
void *accepted(void *arg) {


        // Casting socket argument from void*, sending initial hello
        //      waiting for client response
	int sock = *(int *)arg;
	SlangWrite(sock, "5(HELLO)");
	char *buffer = SlangRead(sock, NULL);
	int guesses;

	// Once ready message is received from client
	while (!strcmp(buffer, "5(READY)")) {


                // Generating random word to be used as the answer
		int randIndex = randNumGen(stringVector.size());
		const string correctAns = stringVector[randIndex];
		SlangWrite(sock, "5(START)");
		string guessWord;
		string response = "5(START)";


		// 6 guesses for the user
		guesses = 1;
		while(guesses <= 6) {


                        // Reading guesses, unless reserved message
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


                                // Resetting guessWord value and building string
                                //      from formatted message
				guessWord = "";
				for(int i = 2; i <= 6; i++) {


					guessWord += buffer[i];
				}


				// Checking validity of supplied word
				if(isValidWord(guessWord)) {


                                        // Checking guess and returning chars
                                        //      corresponding to correct, wrong
                                        //      place, and not in word
					guessWord = SlangCheck(guessWord, correctAns);
					response = "R(" + guessWord + ")";


					// Checking if guessWord is not correct
					//      answer and replying
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
                                        // Does not count as attempt
					response = "5(WRONG)";
				}
				// Sends response string
				SlangWrite(sock, response.c_str());
			}
		}
		SlangRead(sock, buffer);
	}

	// Freeing buffer and closing socket once game is complete
	free(buffer);
	close(sock);
	return NULL;
}

// Function name: main
// Description: Handles command-line arguments pertaining to server settings
//      on execution, sets up initial data structures for word validation
// Parameters: int argc - number of command-line arguments
//      char** argv - array containing command-line arguments
// Return Value: int - return status of program
int main(int argc, char **argv) {
	int portNumber = 0;
	if(argc > 1) {
		portNumber = atoi(argv[1]);

		// Checking that port is in usable range
		if (portNumber < 1024 || portNumber > 65535) {
			printf("%s [port]", argv[0]);
			return EXIT_FAILURE;
		}
	}

	// Reading into both vector and hashmap
	readIntoMap();
	readIntoVector();


	// Initializing SlangLib object and passing address of accepted function
	//      for ultimate handling in library
	SlangLib slang(portNumber, &accepted);
	return 0;
}
