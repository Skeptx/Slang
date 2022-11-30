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
#include <csignal>

#include "slanglib.h"

SlangLib::SlangLib(int portNumber, void *(*accepted)(void *)) : portNumber(portNumber), accepted(accepted) {
	int err = pthread_mutex_init(&m, NULL);
	if (err) {
		fprintf(stderr, "Error: pthread_mutex_init failed: %s\r\n", strerror(err));
		return;
	}
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
		perror("Error: socket failed");
		return;
	}
	struct sockaddr_in *addr = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(INADDR_ANY);
	addr->sin_port = htons(portNumber);
	if (bind(sock, (struct sockaddr *)addr, sizeof(struct sockaddr))) {
		perror("Error: bind failed");
		free(addr);
		return;
	}
	free(addr);
	if (listen(sock, 1280)) {
		perror("Error: listen failed");
		return;
	}
	while (true) {
		int newsock = accept(sock, NULL, NULL);
		if (newsock < 0) {
			perror("Error: accept failed");
			continue;
		}
		err = pthread_mutex_lock(&m);
		if (err) {
			fprintf(stderr, "Error: pthread_mutex_lock failed: %s\r\n", strerror(err));
		}
		pthread_t tid;
		err = pthread_create(&tid, NULL, accepted, &newsock);
		if (err) {
			fprintf(stderr, "Error: pthread_create failed: %s\r\n", strerror(err));
		} else {
			vecOfThreads.push_back(tid);
			vecOfSockets.push_back(newsock);
		}
		err = pthread_mutex_unlock(&m);
		if (err) {
			fprintf(stderr, "Error: pthread_mutex_unlock failed: %s\r\n", strerror(err));
		}
	}
}

SlangLib::SlangLib(int portNumber, char *hostname) : portNumber(portNumber), hostname(hostname) {
	signal(SIGINT, killThreads);
	signal(SIGKILL, killThreads);
	signal(SIGQUIT, killThreads);
	signal(SIGTERM, killThreads);
	signal(SIGTSTP, killThreads);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
		perror("Error: socket failed");
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
		return;
	}
	((struct sockaddr_in *)res->ai_addr)->sin_port = htons(portNumber);
	if (connect(sock, (struct sockaddr *)res->ai_addr, sizeof(struct sockaddr))) {
		perror("Error: connect failed");
		freeaddrinfo(res);
		return;
	}
	freeaddrinfo(res);
}

int SlangLib::getSocket() {
	return sock;
}

char *SlangRead(int sockfd, char * buffer) {
	if (!buffer) {
		buffer = (char *)malloc(14);
	}
	char *i = buffer;
	while (i - buffer < 13) {
		int reading = read(sockfd, i, 1);
		if (reading == 1) {
			++i;
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

        // 4 is correct
        // 2 is wrong position
        // 0 is not in word
        string mutableCorrect = correct;
        int foundIndex = 0;
        for(int i = 0; i < mutableCorrect.length(); i++) {


                if(mutableCorrect[i] == guessed[i]) {


                        guessed[i] = '4';
                        mutableCorrect[i] = ' ';
                }
                else {


                        continue;
                }
        }
        for(int i = 0; i < mutableCorrect.length(); i++) {


                if(guessed[i] != '4') {
					int foundIndex = mutableCorrect.find(guessed[i]);
					if (foundIndex != string::npos && mutableCorrect[i] != guessed[i]) {
                        guessed[i] = '2';
                        mutableCorrect[foundIndex] = ' ';
					} else {
						guessed[i] = '0';
					}
				}
        }
	return guessed;
}

/*static void SlangLib::killThreads(int){

	
}
*/