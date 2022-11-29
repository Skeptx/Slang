#ifndef SLANGLIB_H
#define SLANGLIB_H

#include <string>
#include <vector>

using namespace std;

char *SlangRead(int sockfd, char *buffer);
void SlangWrite(int sockfd, char const * const message);
string SlangCheck(string guessed, const string correct);

class SlangLib {
public:
	SlangLib(int portNumber, void *(*accepted)(void *));
	SlangLib(int portNumber, char *hostname);
	bool isOpen();
	int getSocket();
	void killThreads();
private:
	char *hostname;
	int portNumber;
	vector<pthread_t> vecOfThreads;
	int sock;
	bool open;
	void *(*accepted)(void *);
};

#endif
