/*
 * ftpServer.h
 *
 *  Created on: 2017年12月22日
 *      Author: zhakangning
 */

#ifndef FTPSERVER_H_
#define FTPSERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <syslog.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include "httpClient.h"

using namespace std;

typedef struct ctrlInfo ctrlInfo_t;
struct ctrlInfo{
	int ctrlFd;
	struct sockaddr_in clientAddr;
};

class FtpServer {
public:
	FtpServer(ctrlInfo_t ctrlInfo);
	virtual ~FtpServer();
	int handleSession();
	int closeSocket();
	int parseCmd(string &cmd, int fd);
	int openPASVmode(int connFd, int &listenFd);
	int openPORTmode(string &cmd, int fd);
	int list(int fd);
	int getFile(int fd, const string &fileName);
	string getFileInfo(const string &cmd);
	static string picInfo;
	static string fsuID;
	static string cameraIP;
	static string passwd;
	static pthread_rwlock_t rwlock;
private:
	int m_ctrlFd, m_dataSocket;
	struct sockaddr_in clientAddr;
	bool m_mode;//标志PASV模式是否打开，false表示没开
	unsigned long m_offset;
	string m_picInfo;
};

#endif /* FTPSERVER_H_ */
