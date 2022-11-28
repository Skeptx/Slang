#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>
#include <fstream>
#include <unorder
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include "libslang.h"
using namespace std;


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


int main(int argc, char** argv) {


	if(argc < 2) {


                cout << "./slang-server <port>" << endl;
                exit(EXIT_FAILURE);
	}
	const int portNumber = atoi(argv[1]);
        const string hostName = "acad.kutztown.edu";


        unordered_map<string, bool> stringHash;
        vector<string> stringVector;
	readIntoMap(stringHash);
	readIntoVector(stringVector);
        int randIndex = randNumGen(stringVector.size());


        const string correctAns = stringVector[randIndex];
        cout << "RANDOMLY GENERATED CORRECT ANSWER: " << correctAns << endl;


	SlangLib server('S', portNumber, hostName);
        server.init();


        int sock = server.getSock();
        struct sockaddr_in activeConnection;
        socklen_t infolen = sizeof(activeConnection);
        while(1) {


                int newsockfd = accept(
                                sock,
                                (struct sockaddr *)&activeConnection,
                                &infolen
                        );
                if (newsockfd < 0) {


                        perror("Error in Accepting Connections");
                        continue;
                }

                server.wordleWrite("5(HELLO)");
                string word = server.wordleRead();
                cout << "Read from client: " << word << endl;
                close(newsockfd);
                close(sock);
	}

	return 0;
}
