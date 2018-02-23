/*
 * HttpClient.h
 *
 *  Created on: 2017年12月8日
 *      Author: zhakangning
 */

#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dataHandler.h"

#define MY_HTTP_DEFAULT_PORT 80

using namespace std;
class HttpClient {
public:
	HttpClient(const string &IP="192.168.4.64", const string &password="cdsfITC123");
	virtual ~HttpClient();
	int create(string host, short port);
	int closeSocket();
	string recvData();
	ssize_t recvData(string &data);
	int sendData(const string &data);
	int connectToHost();
	string getAuth(const string &method, const string &uri);
	string search(const unsigned int &pos, string &stat);
	string searchAll();
	string downloadPicture(const string &num);
private:
	int m_socketFd;
	string m_cameraIP;
	short m_port;
	string m_password;
};

#endif /* HTTPCLIENT_H_ */
