/********************************************/
/*   Author: Isaiah Rovenolt                */
/*   Major: Computer Science                */
/*   Creation Date: 11/8/2022               */
/*   Due Date:      11/29/2022              */
/*   Course:  CSC328-010                    */
/*   Rrofessor Name: Dr. Frye               */
/*   Assignment:    World Program           */
/*   Filename:      library.h             */
/*   Purpose:                               */
#ifndef SLANGLIB_H
#define SLANGLIB_H

#include <string>

using namespace std;

class SlangLib {
public:
	SlangLib(char connectionType, void(* readHandler)(char *));
	string wordleRead(int sock);
	void wordleWrite(int sock, string message);
	void init(int portNumber, string hostname);
	void init(int portNumber);
	int errorChecking(int recvCheck,int connectCheck, int sockCheck);
private:
	char connectionType;
	string hostname;
	unsigned short portNumber;
	void(* messageHandler)(char *);
};

#endif
