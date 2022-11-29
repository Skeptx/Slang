#include <arpa/inet.h>
#include <ctype.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <string>

#include "slanglib.h"

SlangLib::SlangLib(int portNumber, void *(*accepted)(void *)) : portNumber(portNumber), accepted(accepted) {
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
		perror("Error: socket failed");
		open = false;
		return;
	}
	struct sockaddr_in *addr = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(INADDR_ANY);
	addr->sin_port = htons(portNumber);
	if (bind(sock, (struct sockaddr *)addr, sizeof(struct sockaddr))) {
		perror("Error: bind failed");
		free(addr);
		open = false;
		return;
	}
	free(addr);
	if (listen(sock, 1280)) {
		perror("Error: listen failed");
		open = false;
		return;
	}
	open = true;
	while (true) {
		int newsock = accept(sock, NULL, NULL);
		if (newsock < 0) {
			perror("Error: accept failed");
			continue;
		}
		pthread_t tid;
		int err = pthread_create(&tid, NULL, accepted, &newsock);
		vecOfThreads.push_back(tid);
		if (err) {
			fprintf(stderr, "Error: pthread_create failed: %s\r\n", strerror(err));
		}
	}
}

SlangLib::SlangLib(int portNumber, char *hostname) : portNumber(portNumber), hostname(hostname) {
	signal(SIGKILL,killThreads);
	signal(SIGTERM,killThreads);
	signal(SIGINT,killThreads);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
		perror("Error: socket failed");
		open = false;
		return;
	}
	struct addrinfo *res = (struct addrinfo *)malloc(sizeof(struct addrinfo));
	struct addrinfo *hints = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
	hints->ai_family = AF_INET;
	hints->ai_socktype = SOCK_STREAM;
	int err = getaddrinfo(hostname, NULL, hints, &res);
	freeaddrinfo(hints);
	if (err) {
		fprintf(stderr, "Error: getaddrinfo failed: %s\r\n", gai_strerror(err));
		freeaddrinfo(res);
		open = false;
		return;
	}
	((struct sockaddr_in *)res->ai_addr)->sin_port = htons(portNumber);
	if (connect(sock, (struct sockaddr *)res->ai_addr, sizeof(struct sockaddr))) {
		perror("Error: connect failed");
		freeaddrinfo(res);
		open = false;
		return;
	}
	freeaddrinfo(res);
	open = true;


	/*string word = wordleRead();
	cout << "CLIENT RECV: \"" << word << "\"\n";
	sleep(1);
	cout << "CLIENT SEND: \"5(READY)\"\n";
	sleep(1);
	wordleWrite("5(READY)");
	close(sock);
	freeaddrinfo(hints);
	freeaddrinfo(baseConnection);
	sleep(1);*/

}

bool SlangLib::isOpen() {
	return open;
}

int SlangLib::getSocket() {
	return sock;
}

char *SlangRead(int sockfd, char * buffer) {
	if (!buffer) {
		buffer = (char *)malloc(9);
	}
	char *i = buffer;
	while (i - buffer < 8) {
		int reading = read(sockfd, i, buffer - i + 8);
		if (reading > 0) {
			i += reading;
			if (i[-1] == ')') {
				break;
			}
		} else {
			if (reading) {
				perror("Error: read failed");
			}
			break;
		}
	}
	i[0] = 0;
	return buffer;
}

void SlangWrite(int sockfd, char const * const message) {
	int len = strlen(message);
	char const *i = message;
	while (i - message < len) {
		int writing = write(sockfd, i, message - i + len);
		if (writing > 0) {
			i += writing;
		} else {
			if (writing) {
				perror("Error: write failed");
			}
			break;
		}
	}
}

string SlangCheck(string guessed, const string correct) {

        // 0 is correct
        // 2 is wrong position
        // 4 is not in word
        string mutableCorrect = correct;
        int foundIndex = 0;
        for(int i = 0; i < mutableCorrect.length(); i++) {


                if(mutableCorrect[i] == guessed[i]) {


                        guessed[i] = '0';
                        mutableCorrect[i] = ' ';
                }
                else {


                        continue;
                }
        }
        for(int i = 0; i < mutableCorrect.length(); i++) {


                if(guessed[i] == '2' || guessed[i] == '0') {


                        continue;
                }
                if((foundIndex = mutableCorrect.find(guessed[i])) !=
                        string::npos && mutableCorrect[i] != guessed[i]) {


                        guessed[i] = '2';
                        mutableCorrect[foundIndex] = ' ';
                }
        }
        for(int i = 0; i < mutableCorrect.length(); i++) {


                if(guessed[i] != '2' && guessed[i] != '0') {


                        guessed[i] = '4';
                }
        }
	return guessed;
}

void SlangLib::killThreads(){

	for(int i = 0; i < vecOfThreads.size(), i++){

		pthread_exit(vecOfThreads[i]);
	}
}
