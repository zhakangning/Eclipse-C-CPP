/*
 * example.h
 *
 *  Created on: 2018年2月13日
 *      Author: root
 */

#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

//函数模板的定义
template <typename T> inline int compare(const T &v1, const T &v2)
{
	if(v1 < v2) return -1;
	//	if(v1 > v2) return 1;
	if(v2 < v1) return 1;//使用尽可能少的操作符，匹配更多的类型，详情见C++ Primer第4版535页
	return 0;
}


//类模板的定义
template <class Type> class Queue {
public:
	Queue();
};


void printValue(char c)
{
	cout<<c<<endl;
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

int setnonblocking(int fd)
{
	int opt = SOCK_NONBLOCK;
	return setsockopt(fd, SOL_SOCKET, SOCK_NONBLOCK, &opt, sizeof(opt));
}

void do_use_fd(int fd)
{
	char buf[1024];
	int rtn = read(fd, buf, sizeof(buf));
	buf[rtn] = 0;
	cout<<buf<<endl;
}

void epollHandle(int epoollFd, struct epoll_event *events, int nFds, int listenFd)
{
	struct epoll_event ev;
	struct sockaddr_in local;
	socklen_t addrLen;
	for(int i = 0; i < nFds; i++){
		if(events[i].data.fd == listenFd){
			int connFd = accept(listenFd, (struct sockaddr *) &local, &addrLen);
			if(-1 == connFd){
				perror("accept");
				return ;
			}
			setnonblocking(connFd);
			ev.events = EPOLLIN | EPOLLET;//设置可读和边缘触发Edge Triggered
			ev.data.fd = connFd;
			if (epoll_ctl(epoollFd, EPOLL_CTL_ADD, connFd, &ev) == -1) {
				perror("epoll_ctl: conn_sock");
				return ;
			}
		}else{
			do_use_fd(events[i].data.fd);
		}
	}
}

void epollTest()
{
	int listen_sock = openServer(8888);
	if(listen_sock == -1) return ;

	int epollFd = epoll_create(10);
	if (epollFd == -1) {
		perror("epoll_create");
		return ;
	}

#define MAX_EVENTS 10
	struct epoll_event ev, events[MAX_EVENTS];
	ev.data.fd = listen_sock;
	ev.events = EPOLLIN;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
		perror("epoll_ctl: listen_sock");
		return ;
	}

	int nFds;
	while(true){
		nFds = epoll_wait(epollFd, events, MAX_EVENTS, -1);
		if (nFds == -1) {
			perror("epoll_pwait");
			return ;
		}
		epollHandle(epollFd, events, nFds, listen_sock);
	}
}

#endif /* EXAMPLE_H_ */
