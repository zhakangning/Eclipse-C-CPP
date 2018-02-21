/*
 * define.h
 *
 *  Created on: 2017年8月8日
 *      Author: zkn
 */

#ifndef DEFINE_H_
#define DEFINE_H_

#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <bitset>
#include <deque>
#include <list>
#include <queue>
#include <stack>
#include <utility>
#include <map>
#include <algorithm>
//#include <pcre.h>

using namespace std;

//#define DEBUG

typedef struct _CHAR2DECIMAL
{
	char c;
	unsigned char bValue;
} CHAR2DECIMAL, *PCHAR2DECIMAL;

string hextostr(string value);
string strtohex(string value);
string strtohex(char value[], int size);
string inttohex(int value, int leng);
int hextoint(string value);
int strtoint(string svalue);
string inttostr(int ivalue);
string floattostr(float ivalue);
unsigned char getcharvalue(char cAscii);

unsigned short GetCRC16(unsigned char *puchMsg, unsigned short usDataLen);
string getchksum(string data);
string get_lenchecksum(string data);
void fileCopy(string filePath);
void sigdemo(int sig);

#endif /* DEFINE_H_ */
