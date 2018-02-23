/*
 * ftpServer.cpp
 *
 *  Created on: 2017年12月22日
 *      Author: zhakangning
 */

#include "ftpServer.h"

//静态成员变量类外定义(类似于全局变量)
string FtpServer::picInfo;
string FtpServer::fsuID;
string FtpServer::cameraIP;
string FtpServer::passwd;
pthread_rwlock_t FtpServer::rwlock;

FtpServer::FtpServer(ctrlInfo_t ctrlInfo) {
	// TODO Auto-generated constructor stub
	m_ctrlFd = ctrlInfo.ctrlFd;
	m_dataSocket = -1;
	m_mode = false;
	m_offset = 0;
}

FtpServer::~FtpServer() {
	// TODO Auto-generated destructor stub
}

int FtpServer::handleSession()
{
	string buf = "220 CDSF FTP server ready.\r\n";
	int ret = send(m_ctrlFd, buf.c_str(), buf.length(), 0);
	if(ret != (int)buf.length()){
		cout<<"send failed\n";
		return -1;
	}
	while(1){
		char cmdBuf[256] = {0};
		//收到0字节表示对方已经关闭了socket
		ret = read(m_ctrlFd, cmdBuf, sizeof(cmdBuf));
		if(1 > ret){
			break;
		}
		cout<<"--------\n";
		cout<<cmdBuf;
		cout<<"\n--------\n";
		string cmd = string(cmdBuf, ret);
		if(-1 == parseCmd(cmd, m_ctrlFd)){
			cout<<"parse cmd error.\n";
			return -1;
		}
	}
	return 0;
}

int FtpServer::parseCmd(string &cmd, int fd)
{
	int rtn = cmd.find("\r");
	if(-1 == rtn) return -1;
	cmd = cmd.substr(0, rtn);//去掉没用的东西
	if(-1 != (int)cmd.find("USER")){
		//331 Password required for ROOT.
		char buf[] = "331 Password required for ROOT.\r\n";
		rtn = send(fd, buf, strlen(buf), 0);

	}

	else if(-1 != (int)cmd.find("PASS")){
		char buf[] = "230 User root logged in.\r\n";
		rtn = send(fd, buf, strlen(buf), 0);

	}

	else if(-1 != (int)cmd.find("SYST")){
		//215 UNIX Type: L8 (Linux)
		char buf[] = "215 UNIX Type: L8 (Linux)\r\n";
		rtn = send(fd, buf, strlen(buf), 0);
	}

	else if(-1 != (int)cmd.find("PWD")){
		char buf[] = "257 \"/PIC\" is current directory.\r\n";
		rtn = send(fd, buf, strlen(buf), 0);
	}

	else if(-1 != (int)cmd.find("CWD")){
		char buf[] = "250 CWD command successful.\r\n";
		rtn = send(fd, buf, strlen(buf), 0);
	}

	else if(-1 != (int)cmd.find("TYPE")){
		if(-1 == (int)cmd.find("A")){
			char buf[] = "200 Type set to I.\r\n";
			rtn = send(fd, buf, strlen(buf), 0);

		}else{
			char buf[] = "200 Type set to A.\r\n";
			rtn = send(fd, buf, strlen(buf), 0);
		}

	}

	else if(-1 != (int)cmd.find("PORT")){
		rtn = openPORTmode(cmd, fd);
	}

	else if(-1 != (int)cmd.find("PASV")){
		int listenFd = -1;//用作PASV模式的监听端口，当成功接到客户端的连接之后就要关闭这个端口
		rtn = openPASVmode(fd, listenFd);
		if(-1 != listenFd){
			close(listenFd);
		}
	}

	else if(-1 != (int)cmd.find("LIST")){
		rtn = list(fd);
		close(m_dataSocket);
	}

	else if(-1 != (int)cmd.find("RETR")){
		string fileName = getFileNameFromCMD(cmd);
		rtn = getFile(fd, fileName);
		close(m_dataSocket);
	}

	else if(-1 != (int)cmd.find("HELP")){
		string buf = "214- The following commands are recognized (* =>'s unimplemented).\r\n";
		buf += "   USER    PORT    STOR    MSAM*   RNTO    NLST    MKD     CDUP\r\n";
		buf += "   PASS    PASV    APPE    MRSQ*   ABOR    SITE    XMKD    XCUP\r\n";
		buf += "   ACCT*   TYPE    MLFL*   MRCP*   DELE    SYST    RMD     STOU\r\n";
		buf += "   SMNT*   STRU    MAIL*   ALLO    CWD     STAT    XRMD    SIZE\r\n";
		buf += "   REIN*   MODE    MSND*   REST    XCWD    HELP    PWD     MDTM\r\n";
		buf += "   QUIT    RETR    MSOM*   RNFR    LIST    NOOP    XPWD\r\n";
		buf += "214 Direct comments to ftp-bugs@CDSF.\r\n";
		rtn = send(fd, buf.c_str(), buf.size(), 0);
	}

	else if(-1 != (int)cmd.find("SIZE")){
		string fileName = getFileNameFromCMD(cmd);
		string buf;
		string fileInfo = getFileInfo(fileName);
		if("" == fileInfo) buf = "550 " + fileName + ": not a plain file.\r\n";
		else buf = "213 "+fileInfo.substr(19) + "\r\n";
		cout<<buf<<endl;
		rtn = send(fd, buf.c_str(), buf.size(), 0);
	}

	else if(-1 != (int)cmd.find("MDTM")){
		string fileName = getFileNameFromCMD(cmd);
		string buf = "550 ";
		string fileInfo = getFileInfo(fileName);
		if("" == fileInfo) buf = "550 " + fileName + ": No such file or directory.\r\n";
		else buf = "213 "+fileInfo.substr(4, 14) + "\r\n";
		rtn = send(fd, buf.c_str(), buf.size(), 0);
	}

	else if(-1 != (int)cmd.find("REST")){
		m_offset = atol(cmd.substr(5).c_str());
		string buf = "350 Restarting at "+ cmd.substr(5) +". Send STORE or RETRIEVE to initiate transfer.\r\n";
		rtn = send(fd, buf.c_str(), buf.size(), 0);
	}

	else if(-1 != (int)cmd.find("QUIT")){
		string buf = "221 Goodbye.\r\n";
		rtn = send(fd, buf.c_str(), buf.size(), 0);
		close(fd);
	}

	else{
		char buf[128];
		snprintf(buf, 128, "500 '%s': command not understood.\r\n", cmd.c_str());
		rtn = send(fd, buf, strlen(buf), 0);
		printf("%s", buf);
	}

	return rtn;
}

int FtpServer::openPASVmode(int connFd, int &listenFd)
{
	listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > listenFd){
		perror("socket");
		return -1;
	}

	struct timeval outtime;
	//set outtime for the data socket
	outtime.tv_sec = 7;
	outtime.tv_usec = 0;
	int opt = SO_REUSEADDR;
	if(0 != setsockopt(listenFd, SOL_SOCKET,SO_RCVTIMEO, &outtime,sizeof(outtime))){
		perror("setsocketopt");
		return -1;
	}
	if(0 != setsockopt(listenFd, SOL_SOCKET,SO_REUSEADDR, &opt,sizeof(opt))){
		perror("setsocketopt2");
		return -1;
	}

	unsigned short port;
	struct sockaddr_in local_host = {0};
	local_host.sin_family = AF_INET;
	local_host.sin_addr.s_addr = 0;
	for(port = 40000; port < 45000; port++){
		local_host.sin_port = htons(port);
		if(0 == bind(listenFd, (struct sockaddr *)&local_host, sizeof(local_host))){
			break;
		}
	}

	if(0 != listen(listenFd, 1)){
		perror("PASV listen");
		return -1;
	}

	//获取被connFd绑定的地址
	struct	sockaddr_in ctrl_addr;
	socklen_t addrlen = sizeof(ctrl_addr);
	if (getsockname(connFd, (struct sockaddr *)&ctrl_addr, &addrlen) < 0) {
		perror("getsockname");
		return -1;
	}
	string bindIP = string(inet_ntoa(ctrl_addr.sin_addr));
	if("" == bindIP){
		cout<<"Get local IP failed.\n";
		return -1;
	}
	int index = bindIP.find(".");
	bindIP.replace(index, 1, ",");
	index = bindIP.find(".", index+1);
	bindIP.replace(index, 1, ",");
	index = bindIP.find(".", index+1);
	bindIP.replace(index, 1, ",");

	string cmdBuf = "227 Entering Passive Mode (";
	cmdBuf += bindIP;
	cmdBuf += ",";
	char num[4];
	snprintf(num, 4, "%d", port >> 8);
	cmdBuf += string(num);
	cmdBuf += ",";
	snprintf(num, 4, "%d", port&0xff);
	cmdBuf += string(num);
	cmdBuf += ").\r\n";
	cout<<"cmdBuf = "<<cmdBuf<<endl;

	//发送命令回复
	if(0 > send(connFd, cmdBuf.c_str(), cmdBuf.length(), 0)){
		perror("PASV send");
		return -1;
	}

	int rtn = 0;
	m_dataSocket = accept(listenFd, 0, 0);
	if(0 > m_dataSocket){
		cmdBuf = "421 Timeout (7 seconds): closing control connection.\r\n";
		rtn = send(connFd, cmdBuf.c_str(), cmdBuf.size(), 0);
	}
	return rtn;
}

int FtpServer::openPORTmode(string &cmd, int fd)
{
	//PORT 192,168,5,1,234,88
	int index = 5;//跳过5个字节开始查找
	for(int i=0; i<4; i++){
		index = cmd.find(",", index);
		index++;
	}
	if(!index){
		cout<<"PORT error.\n";
		return -1;
	}

	int index2 = cmd.find(",", index);
	string port1 = cmd.substr(index, index2-index);
	uint16_t port = atoi(port1.c_str());
	cout<<"port1 = "<<port<<endl;
	port <<= 8;
	index2 ++;
	port += atoi(cmd.substr(index2).c_str());
	cout<<"port = "<<port<<endl;
	m_dataSocket = socket(AF_INET,SOCK_STREAM,0);
	if(m_dataSocket == -1){
		perror("socket");
		return -1;
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = clientAddr.sin_addr.s_addr;
	addr.sin_port = htons(port);

	string buf = "200 PORT command successful.\r\n";
	if(-1 == connect(m_dataSocket, (struct sockaddr *)&addr, sizeof(struct sockaddr))){
		buf = "425 Can't build data connection: Connection timed out.\r\n";
	}
	return send(fd, buf.c_str(), buf.size(), 0);
}

int FtpServer::list(int fd)
{
	string buf = "150 Opening ASCII mode data connection for '/bin/ls'.\r\n";
	if(-1 == send(fd, buf.c_str(), buf.size(), 0)) return -1;

	unsigned long pos, endPos;
	HttpClient client(cameraIP, passwd);

	buf = "MORE";
	unsigned int offset = 0;

	pthread_rwlock_wrlock(&rwlock);
	picInfo.clear();
	while("MORE" == buf){
		buf.clear();
		string picInfo1 = client.search(offset, buf);
		picInfo += picInfo1;
		offset += 200;
		string fileInfo;
		string fileName;
		string lastName;
		int count = 1;
		string fileSize;
		pos = 0;
		while(pos != picInfo1.size()){
			//-rw-r--r--    1 root     root          612 Sep 30  2016 111.log
			//ch01_00000000000002000-192.168.4.64_Camera 01_20180108143327
			pos += 4;
			fileName = picInfo1.substr(pos, 14);
			if(lastName == fileName) count++;//如果文件名一样，序号就加1
			else {lastName = fileName; count = 1;}//如果文件名和上次的文件名不一样，就把序号置为1
			//添加图片序号，一秒中之内可能最多有4张图片
			fileName += "_01.jpg";
			fileName.replace(16, 1, long2str(count));

			fileName.insert(8, "_");
			fileName = fsuID + "_" + fileName;

			fileInfo = "-rw-r--r--    1 root     root             ";
			pos += 15;
			endPos = picInfo1.find("\n", pos);
			fileSize = picInfo1.substr(pos, endPos - pos);//文件大小
			pos = endPos + 1;//把位置移到下一行
			fileInfo.replace(42-fileSize.size(), fileSize.size(), fileSize);
			fileInfo += " Jan  1  1970 ";
			fileInfo += fileName;
			fileInfo += "\r\n";
			cout<<fileInfo;
			if(-1 == send(m_dataSocket, fileInfo.c_str(), fileInfo.length(), 0)){
				pthread_rwlock_unlock(&rwlock);
				return -1;
			}
		}
	}
	pthread_rwlock_unlock(&rwlock);

	buf = "226 Transfer complete.\r\n";
	return send(fd, buf.c_str(), buf.size(), 0);
}


int FtpServer::getFile(int fd, const string &fileName)
{
	string fileInfo = getFileInfo(fileName);
	string buf;
	if("" == fileInfo) buf = "550 " + fileName + ": No such file or directory.\r\n";
	else buf = "150 \r\n";
	if(-1 == send(fd, buf.c_str(), buf.size(), 0)) return -1;

	string numStr = fileInfo.substr(0, 3);

	cout<<"numStr = "<<numStr<<endl;
	HttpClient httpClient(cameraIP, passwd);
	buf = httpClient.downloadPicture(numStr);
	if(-1 == send(m_dataSocket, buf.c_str() + m_offset, buf.size(), 0)) return -1;

	buf = "226 Transfer complete.\r\n";
	return send(fd, buf.c_str(), buf.size(), 0);
}

string FtpServer::getFileInfo(const string &fileName)
{
	cout<<"fileName = "<<fileName<<endl;
	if(37 != (int)fileName.size()){
		return "";
	}

	//000;20180204135532;193818

	string timeStr = fileName.substr(15, 8) + fileName.substr(24, 6);
	//num为图片序号，同一秒钟产生的多张图片用序号来区分
	int num = atoi(fileName.substr(31, 2).c_str());
	unsigned long pos = 0;
	string fileInfo;
	pthread_rwlock_rdlock(&rwlock);
	while(pos < picInfo.size()){
		pos += 4;
		if(timeStr != picInfo.substr(pos, 14)){
			pos += 22;
			continue;
		}

		if(num > 1){
			num --;
			pos += 22;
			continue;
		}

		fileInfo = picInfo.substr(pos-4, picInfo.find("\n", pos) - pos + 4);
		break;
	}
	cout<<"fileInfo = "<<fileInfo<<endl;
	//没有找到对应的文件
	if(pos == picInfo.size()){
		pthread_rwlock_unlock(&rwlock);
		return "";
	}
	pthread_rwlock_unlock(&rwlock);
	return fileInfo;
}

int FtpServer::closeSocket()
{
	return close(m_ctrlFd);
}
