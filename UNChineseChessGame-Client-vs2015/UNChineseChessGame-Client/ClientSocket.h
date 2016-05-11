#pragma once

#define  BUFSIZE 16

extern char recvBuf[BUFSIZE];    // recv buffer
extern char sendBuf[BUFSIZE];    // send buffer

int connectServer();

int recvMsg();

int sendMsg(const char* msg);

void close();
