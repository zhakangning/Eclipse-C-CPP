/*
 * libeventDemo.cpp
 *
 *  Created on: 2018年2月23日
 *      Author: root
 */

#include "define.h"

#define MEM_SIZE    1024

struct event_base* base;
struct sockEvent
{
	struct event* readEvent;
	struct event* writeEvent;
	char* buffer;
};

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

void releaseSockEvent(struct sockEvent* ev)//delete from base and free it
{
	cout<<"releaseSockEvent\n";
	event_del(ev->readEvent);
	free(ev->readEvent);
	free(ev->writeEvent);
	free(ev->buffer);
	free(ev);
}

void handleWrite(int sock, short event, void* arg)
{
	char* buffer = (char*)arg;
	send(sock, buffer, strlen(buffer), 0);

	free(buffer);
}

void handldRead(int sock, short event, void* arg)
{
	//	struct event* writeEvent;
	int size;
	struct sockEvent* ev = (struct sockEvent*)arg;
	ev->buffer = (char*)malloc(MEM_SIZE);
	bzero(ev->buffer, MEM_SIZE);
	size = recv(sock, ev->buffer, MEM_SIZE, 0);
	printf("receive data:%s, size:%d\n", ev->buffer, size);
	if (size == 0) //client has send FIN
	{
		releaseSockEvent(ev);
		close(sock);
		return;
	}
	//add event to base to send the received data
	event_set(ev->writeEvent, sock, EV_WRITE, handleWrite, ev->buffer);
	event_base_set(base, ev->writeEvent);
	event_add(ev->writeEvent, NULL);
}

void handleAccept(int sock, short event, void* arg)//when new connection coming, calling this func
{
	cout<<"connection\n";
	struct sockaddr_in cli_addr;
	int newfd;
	socklen_t sinSize;
	struct sockEvent* ev = (struct sockEvent*)malloc(sizeof(struct sockEvent));
	ev->readEvent = (struct event*)malloc(sizeof(struct event));
	ev->writeEvent = (struct event*)malloc(sizeof(struct event));
	sinSize = sizeof(struct sockaddr_in);
	newfd = accept(sock, (struct sockaddr*)&cli_addr, &sinSize);
	//set the new coming connection event
	event_set(ev->readEvent, newfd, EV_READ|EV_PERSIST, handldRead, ev);
	event_base_set(base, ev->readEvent);
	event_add(ev->readEvent, NULL);
}

void libeventTest()
{
	int sock = openServer(8888);
	struct event listenEvent;
	base = event_base_new();//Create new EventBase
	event_set(&listenEvent, sock, EV_READ|EV_PERSIST, handleAccept, NULL);//conbine listenEvent and  it's callback function
	event_base_set(base, &listenEvent);
	event_add(&listenEvent, NULL);
	event_base_dispatch(base);//start base
}
