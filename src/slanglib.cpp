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



/******************************************************************************/
/*  Function Name: SlangLib()                                                 */
/*                 (constructor)                                              */
/*  Description:  Initializes information needed by server(socket,port,hostname)*/
/*                Will then use that information to bind, then accept         */
/* 				  incoming connections based on the socket information  	  */
/*  Parameters:   int portNumber - the port for the application to run on     */
/*				 void *(accepted)(void *) - void pointer					  */
/*  Return Value:  none                                                       */
/******************************************************************************/
SlangLib::SlangLib(int portNumber, void *(*accepted)(void *)) : portNumber(portNumber), accepted(accepted) {
	int err = pthread_mutex_init(&m, NULL);
	if (err) {
		fprintf(stderr, "Error: pthread_mutex_init failed: %s\r\n", strerror(err));
		return;
	}

	//intialize the socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
		perror("Error: socket failed");
		return;
	}

	//create the sockaddr_in struct
	struct sockaddr_in *addr = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));
	//assign values within the sockaddr_in struct
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(INADDR_ANY);

	//convert port number to network byte order
	addr->sin_port = htons(portNumber);
	if (bind(sock, (struct sockaddr *)addr, sizeof(struct sockaddr))) {
		perror("Error: bind failed");
		free(addr);
		return;
	}
	free(addr);

	//listen for any incomng connections
	if (listen(sock, 1280)) {
		perror("Error: listen failed");
		return;
	}
	while (true) {
		//accept the new connections
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
		//create a thread
		err = pthread_create(&tid, NULL, accepted, &newsock);
		if (err) {
			fprintf(stderr, "Error: pthread_create failed: %s\r\n", strerror(err));
		} else {
			//add one thread into the vector
			vecOfThreads.push_back(tid);

			//add one socket into the vector
			vecOfSockets.push_back(newsock);
		}
		err = pthread_mutex_unlock(&m);
		if (err) {
			fprintf(stderr, "Error: pthread_mutex_unlock failed: %s\r\n", strerror(err));
		}
	}
}



/******************************************************************************/
/*  Function Name: SlangLib()                                                 */
/*                 (constructor)                                              */
/*  Description:  Initializes information needed by client(socket,port,hostname)*/
/*                Will then use that information to bind, then accept         */
/* 				  incoming connections based on the socket information  	  */
/*  Parameters:   int portNumber - the port for the application to run on     */
/*				 char *hostname - hostname to connect server to				  */
/*  Return Value:  none                                                       */
/******************************************************************************/
SlangLib::SlangLib(int portNumber, char *hostname) : portNumber(portNumber), hostname(hostname) {

	//signals for when to call killThreads function when server is running
	signal(SIGINT, killThreads);
	signal(SIGKILL, killThreads);
	signal(SIGQUIT, killThreads);
	signal(SIGTERM, killThreads);
	signal(SIGTSTP, killThreads);
	// Initialize the socket
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

	//connect to the server
	if (connect(sock, (struct sockaddr *)res->ai_addr, sizeof(struct sockaddr))) {
		perror("Error: connect failed");
		freeaddrinfo(res);
		return;
	}
	freeaddrinfo(res);
}



/******************************************************************************/
/*  Function Name: getSocket()                                                */
/*                 			                                                  */
/*  Description:  gets the socket that is assigned to the integer             */
/*                                                                            */
/*  Parameters:    none                                                       */
/*  Return Value:  int - the socket                                           */
/******************************************************************************/
int SlangLib::getSocket() {
	return sock;
}


/******************************************************************************/
/*  Function Name: SlangRead()                                                */
/*                 			                                                  */
/*  Description:  read function that will read data sent over connection      */
/*				  will read the information sent until ')' is reached 		  */
/*                                                                            */
/*  Parameters:    int sockfd - the socket                                    */
/*				   char * buffer - the buffer to read information sent        */
/*  Return Value:  char* - the information sent by either client/server       */
/******************************************************************************/
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



/******************************************************************************/
/*  Function Name: SlangWrite()                                               */
/*                 			                                                  */
/*  Description:  write function that will read data sent over connection     */
/*				  will read the information send until ')' is reached 		  */
/*                                                                            */
/*  Parameters:    int sockfd - the socket                                    */
/*				   char * message - the message to write to client/server     */
/*  Return Value:  none 												      */
/******************************************************************************/
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



/******************************************************************************/
/*  Function Name: SlangCheck()                                               */
/*                 			                                                  */
/*  Description:  checks the word guessed from user against the correct word  */
/*				  It will compare the two and return 0 if in correct position */
/*				  2 on the first character(if double) in wrong position 	  */
/*				  and 4 if the character is not in the word
/*                                                                            */
/*  Parameters:    int sockfd - the socket                                    */
/*				   char * message - the message to write to client/server     */
/*  Return Value:  none 												      */
/******************************************************************************/

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
