#ifndef SLANGLIB_H
#define SLANGLIB_H

#include <string>
#include <vector>
#include <pthread.h>
#include <bits/stdc++.h>
#include <sys/socket.h>

using namespace std;

char *SlangRead(int sockfd, char *buffer);
void SlangWrite(int sockfd, char const * const message);
string SlangCheck(string guessed, const string correct);

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static vector<pthread_t> vecOfThreads;
static vector<int> vecOfSockets;
static int sock;

class SlangLib {
public:
	SlangLib(int portNumber, void *(*accepted)(void *));
	SlangLib(int portNumber, char *hostname);
	int getSocket();
	static void killThreads(int) {
		int err = pthread_mutex_lock(&m);
		if (err) {
			fprintf(stderr, "Error: pthread_mutex_lock failed: %s\r\n", strerror(err));
		}
		for (int i = 0; i < vecOfThreads.size(); ++i) {
			SlangWrite(vecOfSockets[i], "5(DEATH)");
			close(vecOfSockets[i]);
			pthread_cancel(vecOfThreads[i]);
		}
		err = pthread_mutex_destroy(&m);
		if (err) {
			fprintf(stderr, "Error: pthread_mutex_destroy failed: %s\r\n", strerror(err));
			exit(EXIT_FAILURE);
		}
		close(sock);
		signal(SIGINT, SIG_DFL);
		signal(SIGKILL, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGTERM, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		exit(EXIT_SUCCESS);
	};
private:
	char *hostname;
	int portNumber;
	void *(*accepted)(void *);
};

#endif
