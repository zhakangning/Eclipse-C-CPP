/*
 * dataHandler.h
 *
 *  Created on: 2018年1月4日
 *      Author: zhakangning
 */

#ifndef DATAHANDLER_H_
#define DATAHANDLER_H_

#include <stdio.h>
#include <iostream>
using namespace std;

string getMD5(const string &srcStr);
string getAuthorization(const string &data_recv, const string &method, const string &uri);
string long2str(long n);
string float2str(float n);
void insertStr(string &src, const string &flag, const string &subStr);
string subStr(const string &src, const string &flag, const unsigned long &n);
string subStr(const string &src, const string &head, const string &end);
string getTime();
string convertTimeNum2str(const time_t &nTime);
string getFileNameFromCMD(const string &cmd);

#endif /* DATAHANDLER_H_ */
