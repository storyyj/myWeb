#pragma once
#include <map>
class EventLoop;
class Socket;
class Acceptor;
class Connection;

class Server
{
private:
    /* data */
    EventLoop *loop;
    Acceptor *acceptor;
    std::map<int,Connection*> connectionsMap;
public:
    Server(EventLoop*);
    ~Server();
    void handleReadEvent(int);
    void newConnection(Socket *sock);
    void deleteConnection(Socket *sock);
};


