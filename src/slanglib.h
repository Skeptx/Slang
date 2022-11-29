#ifndef SLANGLIB_H
#define SLANGLIB_H

#include <string>
#include <vector>

using namespace std;

char *SlangRead(int sockfd, char *buffer);
void SlangWrite(int sockfd, char const * const message);
string SlangCheck(string guessed, const string correct);

static vector<pthread_t> vecOfThreads;

class SlangLib {
public:
	SlangLib(int portNumber, void *(*accepted)(void *));
	SlangLib(int portNumber, char *hostname);
	bool isOpen();
	int getSocket();
	static void killThreads(int) {
		for (int i = 0; i < vecOfThreads.size(); i++) {
			pthread_cancel(vecOfThreads[i]);
		}
	};
private:
	char *hostname;
	int portNumber;
	int sock;
	bool open;
	void *(*accepted)(void *);
};

#endif
