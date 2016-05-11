//#include "stdafx.h"
#include "Define.h"
#include "ClientSocket.h"
#include <windows.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

SOCKADDR_IN server;       // server address
SOCKET client;            // client socket
char recvBuf[BUFSIZE];    // recv buffer
char sendBuf[BUFSIZE];    // send buffer

int connectServer()
{
    std::cout << "Connect server: " << SERVER_IP << ":" << SERVER_PORT << std::endl;
	int rtn = 0, err = 0;
	WSADATA wsaData;
	// initialize windows socket library      
	err = WSAStartup(0x0202, &wsaData);
	if (err != NO_ERROR)
	{
		std::cout << "Failed with WSAStartup error: " << err << std::endl;
		rtn = 1;
		return rtn;
	}
	// creat socket     
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client == INVALID_SOCKET)
	{
		std::cout << "Socket failed with error: " << WSAGetLastError() << std::endl;
		rtn = 2;
		return rtn;
	}
	// server address    
	memset(&server, 0, sizeof(SOCKADDR_IN));
	server.sin_family = PF_INET;
	server.sin_port = htons(SERVER_PORT);
	server.sin_addr.s_addr = inet_addr(SERVER_IP);
	// connect     
	err = connect(client, (struct sockaddr *) &server, sizeof(SOCKADDR_IN));
	if (err < 0)
	{
		std::cout << "Connect failed with error: " << err << std::endl;
		rtn = 3;
		return rtn;
	}
	return rtn;
}

int recvMsg()
{
	int rtn = 0, err = 0;
	memset(recvBuf, 0, BUFSIZE);
	err = recv(client, recvBuf, BUFSIZE, 0);
	if (err < 0)
	{
		std::cout << "Receive msg failed with error: " << err << std::endl;
		rtn = 1;
		return rtn;
	}
	return rtn;
}

int sendMsg(const char* msg)
{
	int rtn = 0, err = 0;
	int len = strlen(msg);
	len = len < BUFSIZE ? len : BUFSIZE;
	memset(sendBuf, 0, BUFSIZE);
	memcpy(sendBuf, msg, len);
	err = send(client, sendBuf, BUFSIZE, 0);
	if (err < 0)
	{
		std::cout << "Send msg failed with error: " << err << std::endl;
		rtn = 1;
		return rtn;
	}
	return rtn;
}

void close()
{
	// close socket    
	closesocket(client);
	// WSA clean
	WSACleanup();
	std::cout << "Close socket" << std::endl;
}