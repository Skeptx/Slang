#ifndef SLANGLIB_H
#define SLANGLIB_H

#include <string>

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
private:
	char *hostname;
	int portNumber;
	int sock;
	bool open;
	void *(*accepted)(void *);
};

#endif