#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QObject>
#include <QtNetwork/QTcpSocket>

#define  BUFSIZE 16

extern char recvBuf[BUFSIZE];    // recv buffer
extern char sendBuf[BUFSIZE];    // send buffer

int connectServer();

int recvMsg();

int sendMsg(const char* msg);

void close();

class ClientSocket : public QObject
{
    Q_OBJECT


private:
    QTcpSocket *tcpSocket;

public:
    ClientSocket();
    void recvMsg();
    int sendMsg(const char* msg);
    void displayError(QAbstractSocket::SocketError);
    int connectServer();
    void close();

};

#endif // CLIENTSOCKET_H
