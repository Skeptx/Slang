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
	SlangLib(char connectionType, int portNumber, string hostname);
	string wordleRead(int sock);
	void wordleWrite(int sock, string message);
	void init();
	void cliConnect();
	void servConnect();
private:
	char connectionType;
	string hostname;
	int portNumber;
	int sock;
	void(* messageHandler)(char *);
};

#endif
