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

#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;

class SlangLib{

public:
    SlangLib(char connectionType, int portNumber,string hostname);

    void wordleRead(int sock);

    void wordleWrite(int sock);

    void connection();

    int errorChecking(int recvCheck,int connectCheck, int sockCheck);

private:
        char connectionType;
        unsigned short portNumber;
        string hostname;
};



#endif
