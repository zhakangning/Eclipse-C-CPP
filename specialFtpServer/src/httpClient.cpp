/*
 * HttpClient.cpp
 *
 *  Created on: 2017年12月8日
 *      Author: zhakangning
 */

#include "httpClient.h"

HttpClient::HttpClient(const string &IP, const string &password)
{
	m_socketFd = -1;
	m_cameraIP = IP;
	m_port = 80;
	m_password = password;
}

HttpClient::~HttpClient() {
	// TODO Auto-generated destructor stub
}

string HttpClient::recvData()
{
	char buf[1024] = {0};
	string str;
	int ret;
	while((ret = recv(m_socketFd, buf, 1024, 0)) > 0){
		str += string(buf, ret);
		bzero(buf, 1024);
	}
	if(-1 == ret){
		perror("recv");
		return "";
	}

	return str;
}

/*
 * 此函数可能会存在一个问题
 * 当接收的数据达到size_t类型能表示的最大值时，
 * 函数的返回值为-1，因为无符号转有符号数。
 * 当函数执行出错的时，函数的返回值也为-1，
 * 所以就会认为接收到最大数据量时是出错的情况
 */
ssize_t HttpClient::recvData(string &data)
{
	data.clear();
	char buf[512] = {0};
	int ret;
	while((ret = recv(m_socketFd, buf, 512, 0)) > 0){
		data += string(buf, ret);
	}
	if(-1 == ret){
		perror("recv");
		close(m_socketFd);
		m_socketFd = -1;
		return -1;
	}
	return data.size();
}

int HttpClient::sendData(const string &data)
{
	int sent=0,tmpres=0;
	int size = data.length();

	while(sent < size){
		tmpres = send(m_socketFd, data.substr(sent).c_str(), size - sent ,0);
		if(tmpres == -1){
			close(m_socketFd);
			m_socketFd = -1;
			return -1;
		}
		sent += tmpres;
	}
	return sent;
}


int HttpClient::connectToHost()
{
	m_socketFd = socket(AF_INET,SOCK_STREAM,0);
	if(m_socketFd == -1){
		perror("socket");
		return -1;
	}
	struct sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
	inet_aton(m_cameraIP.c_str(), &client_addr.sin_addr);
	client_addr.sin_port = htons(m_port);

	if(connect(m_socketFd, (struct sockaddr *)&client_addr, sizeof(struct sockaddr)) == -1){
		perror("connect");
		close(m_socketFd);
		m_socketFd = -1;
	}
	return m_socketFd;
}

int HttpClient::closeSocket()
{
	return close(m_socketFd);
}

string HttpClient::getAuth(const string &method, const string &uri)
{
	string data = method+" "+uri+" HTTP/1.1\r\n\r\n";
	if(connectToHost() < 0){
		printf("http_tcpclient_create failed\n");
		return "";
	}
	if(sendData(data) < 0){
		printf("http_tcpclient_send failed..\n");
		return "";
	}
	string data_recv = recvData();
	if("" == data_recv){
		printf("http_tcpclient_recv failed.\n");
		return "";
	}
	closeSocket();
	//	cout<<data_recv<<endl;
	string nonce = subStr(data_recv, "nonce=\"", "\"");
	cout<<"nonce = "<<nonce<<endl;

	string srcStr = "admin:DX-231T-D:"+m_password;
	string h1 = getMD5(srcStr);
	cout<<"h1 = "<<h1<<endl;
	srcStr = method+":"+uri;
	string h2 = getMD5(srcStr);
	cout<<"h2 = "<<h2<<endl;
	srcStr = h1+":"+nonce+":"+h2;
	string response = getMD5(srcStr);

	string authorization = "Digest username=\"admin\",realm=\"DX-231T-D\",nonce=\"\",uri=\"\",response=\"\"";
	unsigned long pos = authorization.find("nonce=")+7;
	authorization.insert(pos, nonce);
	pos = authorization.find("uri=", pos)+5;
	authorization.insert(pos, uri);
	authorization.insert(authorization.size()-1, response);

	return authorization;
}

/*
 * 查询存储卡中的图片信息
 * 返回图片的url
 */
string HttpClient::search(const unsigned int &pos, string &stat)
{
	//获取最近30天的照片,一次接收的数量不能超过200，否则会超过http包容纳数据字节数的最大限度
	string xmlData = "<?xml version:\"1.0\" encoding=\"utf-8\"?>"
			"<CMSearchDescription>"
			"<searchID>C7CDA42E-2B80-0001-498B-8F892E70148E</searchID>"
			"<trackIDList><trackID>103</trackID></trackIDList>"
			"<timeSpanList><timeSpan>"
			"<startTime>" + convertTimeNum2str(time(0)-3600*24*30) + "Z</startTime>"
			"<endTime>2024-09-10T00:00:00Z</endTime>"
			"</timeSpan></timeSpanList>"
			"<contentTypeList><contentType>metadata</contentType></contentTypeList>"
			"<maxResults>200</maxResults>"
			"<searchResultPostion>"+ long2str(pos) +"</searchResultPostion>"
			"<metadataList><metadataDescriptor>//recordType.meta.std-cgi.com/MANUAL</metadataDescriptor></metadataList>"
			"</CMSearchDescription>";
	string data = "POST /ISAPI/ContentMgmt/search HTTP/1.1\r\n";
	data += "Authorization:" + getAuth("POST", "/ISAPI/ContentMgmt/search") + "\r\n";
	data += "Content-Length:" + long2str(xmlData.size()) + "\r\n\r\n";
	data += xmlData;

	//	cout<<data<<endl;
	if(connectToHost() < 0){
		printf("http_tcpclient_create failed.\n");
		return "";
	}
	if(sendData(data) < 0){
		printf("http_tcpclient_send failed.\n");
		return "";
	}
	if(-1 == recvData(data)){
		printf("http_tcpclient_recv failed.\n");
		return "";
	}
	closeSocket();

	//取出获取状态，用于检查是否取完
	stat = subStr(data, "<responseStatusStrg>", "</responseStatusStrg>");
	cout<<data.substr(0, data.find("<matchList>"))<<endl;
	string picInfo;
	unsigned long index = 0;
	while(1){
		index = data.find("<playbackURI>", index);
		if(string::npos == index){
			break;
		}
		index += 13;
		string uri = data.substr(index, data.find("</playbackURI>", index) - index);
		picInfo += uri.substr(uri.find("name=") + 22, 3) + ";";//取图片序号
		picInfo += uri.substr(uri.find("Camera") + 10, 14) + ";";//取时间
		picInfo += uri.substr(uri.find("size=") + 5);//取图片大小
		picInfo += "\n";
	}
	cout<<picInfo<<endl;

	return picInfo;
}

string HttpClient::searchAll()
{
	string stat = "MORE";
	unsigned int pos = 0;
	string picInfo;
	while(stat == "MORE"){
		picInfo += search(pos, stat);
		pos += 200;
	}
	return picInfo;
}


/*
 * 通过图片序号下载图片
 */
string HttpClient::downloadPicture(const string &num)
{
	string data = "GET /ISAPI/ContentMgmt/download HTTP/1.1\r\n";
	data += "Authorization:"+getAuth("GET", "/ISAPI/ContentMgmt/download");
	data += "\r\nContent-Length:273\r\n\r\n";
	data += "<?xml version='1.0'?><downloadRequest><playbackURI>rtsp:"
			"//192.168.4.64/Streaming/tracks/120?starttime=2018-01-18T06:38:58Z&amp;endtime=2018-01-18T06:38:58Z&amp;name=ch01_000000000000";
	data += num + "00-192.168.4.64_Camera 01_20180118143858&amp;size=201742</playbackURI></downloadRequest>";
	cout<<data<<endl;
	if(connectToHost() < 0){
		printf("Connect failed.\n");
		return "";
	}
	if(sendData(data) < 0){
		printf("Send failed.\n");
		return "";
	}

	if(-1 == recvData(data)){
		printf("Recv failed.\n");
		return "";
	}
	closeSocket();

	if(string::npos == data.find("200 OK")) return "";

	cout<<"Download picture OK.\n";
	return subStr(data, "\r\n\r\n", string::npos);
}
