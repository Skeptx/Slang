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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include "library.h"

SlangLib::SlangLib(char c, int p, string h){
    switch(c){
        case 's':
        case 'S':
            connectionType = 'S';
			break;
        case 'c':
        case 'C':
            connectionType = 'C';
			break;
        default:
            cout << "Error: Invalid Connection Type" << endl;
            exit(EXIT_FAILURE);
    }
    portNumber = p;
	hostname = h;
}

void SlangLib :: wordleRead(int sock){
    char *receiveHello;
    int receiving = recv(sock,receiveHello, strlen(receiveHello),0);
    if(receiving == -1){
        perror("Error receiving Message");
        exit(EXIT_FAILURE); // exit on failure of receiving message
    }

}

void SlangLib :: wordleWrite(int sock){
    string sayHello = "HELLO";
    int sending = send(sock,sayHello.c_str(),sayHello.length(),0);
    if(sending == -1){
        perror("Error Sending Message");
        exit(EXIT_FAILURE); // exit on failure of sendng message
    }
}

void SlangLib :: connection(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        perror("Socket Error\n");
        exit(EXIT_FAILURE); //exit upon failure of socket creation
    }
    if(connectionType == 'S'){
		struct sockaddr_in baseConnection = {AF_INET, portNumber, INADDR_ANY};

		/*static struct sigaction act;
		act.sa_handler = catcher;
		sigfillset(&(act.sa_mask));
		sigaction(SIGPIPE, &act, NULL);*/

        int binding = bind(sock, (struct sockaddr *)&baseConnection, sizeof(struct sockaddr));
        if(binding == -1){
            perror("Error in Bind Call:\n");
            exit(EXIT_FAILURE); // exit on failure of bind call
        }
        int listening = listen(sock, 2);
        if(listening == -1){
            perror("Error in Listen Call:\n");
            exit(EXIT_FAILURE); // exit on failure of listen call
        }
		socklen_t socklen = sizeof(struct sockaddr);
        while(true){
            int accepting = accept(sock,(struct sockaddr *)&baseConnection, &socklen);
            if(accepting == -1){
                perror("Error in Accepting Connections:\n");
                exit(EXIT_FAILURE); //exit on failure of listen call
            }
			wordleWrite(sock);
			wordleRead(sock);
        }
    }
    else{
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
			exit(EXIT_FAILURE); // exit on failure of getaddrinfo
		}
		((struct sockaddr_in *)baseConnection->ai_addr)->sin_port = htons(portNumber);
		int connection = connect(sock, (struct sockaddr *)baseConnection->ai_addr,sizeof(struct sockaddr));
		if(connection == -1){
			perror("Connection Error\n:");
			freeaddrinfo(hints);
			freeaddrinfo(baseConnection);
			exit(EXIT_FAILURE); /// exit  on failure of connection call
		}
        wordleWrite(sock);
        wordleRead(sock);
		freeaddrinfo(hints);
		freeaddrinfo(baseConnection);
    }
}

int SlangLib :: errorChecking(int recvCheck,int connectCheck, int sockCheck){
    if(recvCheck == -1){
        perror("Error in receiving message\n");
        return -1;
    } // check when call recv

    if(connectCheck == -1){
        perror("Error Connecting\n");
        return -1;
    } // check when call connect

    if(sockCheck == -1){
        perror("Error in Creating Socket\n");
        return -1;
    } // check when socket is created
    return 0;
}
