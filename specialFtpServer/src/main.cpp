/*
 ============================================================================
 Name        : main.cpp
 Author      : zhakangning
 Version     : 1.0
 ============================================================================
 */

#include "ftpServer.h"
#include <signal.h>
#include <pthread.h>

int getCtrlInfo()
{
	ifstream in("/cdsf/camera/configure");
	if(!in){
		cout<<"'configure' file is not exist.\n";
		return -1;
	}
	string buf;
	while(in){
		in>>buf;
		if("CameraIP" == buf){
			in>>buf>>FtpServer::cameraIP;
			in>>buf;
		}

		if("Password" == buf){
			in>>buf>>FtpServer::passwd;
		}
	}
	in.close();

	in.open("/cdsf/rtuinfoz.conf");
	if(!in){
		cout<<"'rtuinfoz.conf' file is not exist.\n";
		return -1;
	}
	in>>FtpServer::fsuID;
	in.close();
	FtpServer::fsuID = FtpServer::fsuID.substr(6);
	cout<<"fsuID = "<<FtpServer::fsuID<<endl;
	cout<<"cameraIP = "<<FtpServer::cameraIP<<endl;
	cout<<"passwd = "<<FtpServer::passwd<<endl;
	return 0;
}

int openServer(const unsigned short &port)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == fd){
		perror("socket");
		return -1;
	}

	int opt = SO_REUSEADDR;
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))!= 0){
		perror("setsockopt");
		return -1;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if(-1 == bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)))
	{
		perror("bind");
		return -1;
	}
	if(listen(fd, 24) == -1){
		perror("listen");
		return -1;
	}
	cout<<"ftp server started, listen port "<<port<<endl;
	return fd;
}

void * handleSession(void *arg)
{
	FtpServer *server = (FtpServer *)arg;
	server->handleSession();
	cout<<"客户端退出\n";
	delete server;
	return 0;
}


int main(int argc, char * argv[])
{
	cout<<"Version: 1.1\n";

	if(pthread_rwlock_init(&FtpServer::rwlock, 0)){
		cout<<"pthread_rwlock_init failed.\n";
		return -1;
	}

	ctrlInfo_t ctrlInfo;
	if(getCtrlInfo()){
		return -1;
	}

	short port = 8888;
	if(argc > 1){
		port = atoi(argv[1]);
	}

	int fd = openServer(port);
	if(-1 == fd){
		return -1;
	}

	HttpClient client(FtpServer::cameraIP, FtpServer::passwd);
	FtpServer::picInfo = client.searchAll();

	socklen_t addrLen;
	while(1){
		addrLen = sizeof(struct sockaddr);
		bzero(&ctrlInfo, sizeof(ctrlInfo));
		ctrlInfo.ctrlFd = accept(fd, (struct sockaddr *)&ctrlInfo.clientAddr, &addrLen);
		if(0 > ctrlInfo.ctrlFd){
			perror("accept");
			break;
		}

		FtpServer *server = new FtpServer(ctrlInfo);
		pthread_t childThread;
		if (pthread_create(&childThread, 0, handleSession, server))
		{
			cout<<"Create thread failed.\n";
		}
		pthread_detach(childThread);

	}

	return EXIT_SUCCESS;
}
