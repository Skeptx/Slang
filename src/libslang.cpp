/********************************************/
/*   Author: Isaiah Rovenolt                */
/*   Major: Computer Science                */
/*   Creation Date: 11/8/2022               */
/*   Due Date:      11/29/2022              */
/*   Course:  CSC328-010                    */
/*   Rrofessor Name: Dr. Frye               */
/*   Assignment:    Wordle Program          */
/*   Filename:      library.cpp           */
/*   Purpose:                               */
#include <iostream>
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

SlangLib::SlangLib(char connectionType, int portNumber, string hostname) :
        connectionType(toupper(connectionType)),
        portNumber(portNumber), hostname(hostname) {


        if(connectionType != 'C' && connectionType != 'S') {


                cout << "ERROR: Invalid connection type!" << endl;
                exit(EXIT_FAILURE);
        }
}

string SlangLib::wordleRead(int sock){
    char *recvChar = (char *)malloc(10);
	read(sock, recvChar, 9);
	string recv(recvChar);
	free(recvChar);
	return recv;
}

void SlangLib::wordleWrite(int sock, string message){
    int sending = send(sock, message.c_str(), message.length(), 0);
    if(sending == -1){
        perror("Error Sending Message");
    }
}

void SlangLib::init() {


        switch(connectionType) {


        case 'C':


                cliConnect();
                break;
        case 'S':


                servConnect();
                break;
        default:


                cout << "ERROR: Invalid connection type!" << endl;
                exit(EXIT_FAILURE);
                break;
        }
}
void SlangLib::cliConnect(){


    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        perror("Socket Error\n");
        exit(EXIT_FAILURE);
    }
	struct addrinfo *baseConnection = (struct addrinfo *)malloc(sizeof(struct addrinfo));
	struct addrinfo *hints = (struct addrinfo *)malloc(sizeof(struct addrinfo));
	hints->ai_family = AF_INET;
	hints->ai_flags = 0;
	hints->ai_protocol = 0;
	hints->ai_socktype = SOCK_STREAM;
	int getConnectionInfo = getaddrinfo(hostname.c_str(), NULL, hints, &baseConnection);
	if(getConnectionInfo){
		printf("Error in Calling getaddrinfo: %s\n", gai_strerror(getConnectionInfo));
		freeaddrinfo(hints);
		freeaddrinfo(baseConnection);
		exit(EXIT_FAILURE);
	}
	((struct sockaddr_in *)baseConnection->ai_addr)->sin_port = htons(portNumber);
	int connection = connect(sock, (struct sockaddr *)baseConnection->ai_addr,sizeof(struct sockaddr));
	if(connection == -1){
		perror("Connection Error\n:");
		freeaddrinfo(hints);
		freeaddrinfo(baseConnection);
		exit(EXIT_FAILURE);
	}
	string word = wordleRead(sock);
	cout << "CLIENT RECV: " << word << endl;
	sleep(1);
	cout << "CLIENT SEND: 5(READY)\n";
	sleep(1);
	wordleWrite(sock, "5(READY)");
	close(sock);
	freeaddrinfo(hints);
	freeaddrinfo(baseConnection);
	sleep(1);
}

void SlangLib::servConnect() {


    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        perror("Socket Error\n");
        exit(EXIT_FAILURE);
    }
	struct sockaddr_in baseConnection, activeConnection;
	memset(&baseConnection, 0, sizeof(struct sockaddr_in));
	baseConnection.sin_family = AF_INET;
	baseConnection.sin_addr.s_addr = htonl(INADDR_ANY);
	baseConnection.sin_port = htons(portNumber);
	int binding = bind(sock, (struct sockaddr *)&baseConnection, sizeof(struct sockaddr));
	if(binding == -1){
		perror("Error in Bind Call:\n");
		exit(EXIT_FAILURE);
	}
	int listening = listen(sock, 5);
	if(listening == -1){
		perror("Error in Listen Call:\n");
		exit(EXIT_FAILURE);
	}
	socklen_t infolen = sizeof(activeConnection);
	while (true) {
		int newsockfd = accept(sock, (struct sockaddr *)&activeConnection, &infolen);
		if (newsockfd < 0) {
			perror("Error in Accepting Connections");
			continue;
		}
		/*pthread_t tid;
		int err = pthread_create(&tid, NULL, connectionHandler, &tid));
		if (err) {
			printf("Error: pthread_create failed: %s\n", strerror(err));
		}*/
		wordleWrite(newsockfd, "5(HELLO)");
		string word = wordleRead(newsockfd);
		cout << "Read from client: " << word << endl;
		close(newsockfd);
	}
}
