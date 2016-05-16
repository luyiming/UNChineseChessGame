//#include "stdafx.h"
#include "Define.h"
#include "ClientSocket.h"
#include <windows.h>
#include <iostream>
#include <QDebug>
#include <QtNetwork/QTcpSocket>

#pragma comment(lib, "Ws2_32.lib")

SOCKADDR_IN server;       // server address
SOCKET client;            // client socket
char recvBuf[BUFSIZE];    // recv buffer
char sendBuf[BUFSIZE];    // send buffer

int connectServer()
{
    qDebug() << "Connect server: " << SERVER_IP << ":" << SERVER_PORT;
	int rtn = 0, err = 0;
	WSADATA wsaData;
	// initialize windows socket library      
	err = WSAStartup(0x0202, &wsaData);
	if (err != NO_ERROR)
	{
        qDebug() << "Failed with WSAStartup error: " << err;
		rtn = 1;
		return rtn;
	}
	// creat socket     
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client == INVALID_SOCKET)
	{
        qDebug() << "Socket failed with error: " << WSAGetLastError();
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
        qDebug() << "Connect failed with error: " << err;
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
        qDebug() << "Receive msg failed with error: " << err;
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
        qDebug() << "Send msg failed with error: " << err;
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
    qDebug() << "Close socket";
}

void ClientSocket::ClientSocket()
{
    this->tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &ClientSocket::recvMsg);
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
            this,SLOT(displayError(QAbstractSocket::SocketError)));
}


void ClientSocket::recvMsg()
{
    memset(recvBuf, 0, BUFSIZE);
    tcpSocket->read(recvBuf, BUFSIZE);
    qDebug() << recvBuf;
}

int ClientSocket::sendMsg(const char* msg)
{
    int len = strlen(msg);
    len = len < BUFSIZE ? len : BUFSIZE;
    memset(sendBuf, 0, BUFSIZE);
    memcpy(sendBuf, msg, len);
    tcpSocket->write(sendBuf, BUFSIZE);
    qDebug() << sendBuf;
}

void ClientSocket::displayError(QAbstractSocket::SocketError)
{
    qDebug() << tcpSocket->errorString();
}

int ClientSocket::connectServer()
{
    qDebug() << "Connect server: " << SERVER_IP << ":" << SERVER_PORT;
    tcpSocket->abort();
    tcpSocket->connectToHost(QString(SERVER_IP), QString(SERVER_PORT));
}

void ClientSocket::close()
{
    tcpSocket->close();
    qDebug() << "Close socket";
}
