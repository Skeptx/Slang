// Author: Isaiah Rovenolt (with help from Henry Morales and Patrick Perrin)
// Major: Computer Science
// Creation Date: 10/24/2022
// Due Date: 11/29/2022
// Course: CSC328
// Professor: Dr. Frye
// Assignment: NWProgram - Wordle
// File Name: slanglib.h
// Purpose: This is the library h file for the NWProgram Wordle Assignment. This has functions
// 			that both client and server files will utilize.  It initializes client and server
//			information to communicate with each other.



#ifndef SLANGLIB_H
#define SLANGLIB_H

#include <string>
#include <vector>
#include <pthread.h>
#include <bits/stdc++.h>
#include <sys/socket.h>

using namespace std;
// read function for server/client
char *SlangRead(int sockfd, char *buffer);

// write function for servee/client
void SlangWrite(int sockfd, char const * const message);

//checks a guessed word against the correct word
string SlangCheck(string guessed, const string correct);

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
// vector of thread ids for clients
static vector<pthread_t> vecOfThreads;
// vector of sockets
static vector<int> vecOfSockets;
// a socket
static int sock;

class SlangLib {
public:
	// server constructor
	SlangLib(int portNumber, void *(*accepted)(void *));

	// client constructor
	SlangLib(int portNumber, char *hostname);

	//gets the integer assigned to the socket
	int getSocket();

	//kill threads and cancels specified thread used by one client
	static void killThreads(int) {
		int err = pthread_mutex_lock(&m);
		if (err) {
			fprintf(stderr, "Error: pthread_mutex_lock failed: %s\r\n", strerror(err));
		}
		for (int i = 0; i < vecOfThreads.size(); ++i) {
			SlangWrite(vecOfSockets[i], "5(DEATH)");
			//close individual client thread ID
			close(vecOfSockets[i]);
			pthread_cancel(vecOfThreads[i]);
		}
		err = pthread_mutex_destroy(&m);
		if (err) {
			fprintf(stderr, "Error: pthread_mutex_destroy failed: %s\r\n", strerror(err));
			//exit on failure
			exit(EXIT_FAILURE);
		}
		//close the socket
		close(sock);
		signal(SIGINT, SIG_DFL);
		signal(SIGKILL, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGTERM, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		//exit on sucess
		exit(EXIT_SUCCESS);
	};
private:
	//hostname to connect to
	char *hostname;
	//port number to be converted to network byte order
	int portNumber;
	//void pointer accepted
	void *(*accepted)(void *);
};

#endif
