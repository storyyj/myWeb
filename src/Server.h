#pragma once
#include <map>
#include <vector>
#include <functional>
class EventLoop;
class Socket;
class Acceptor;
class Connection;
//将threadpool类由eventloop掌管改为server掌管
class ThreadPool;

class Server
{
private:
    /* data */
    EventLoop *mainReactor;  //只负责接受连接，然后将新到的连接分发给一个subReactor
    Acceptor *acceptor;  //连接接受器
    std::map<int,Connection*> connectionsMap;  //TCP连接，一个连接对应一个subReactor，一个subReactor对应一个线程
    std::vector<EventLoop*> subReactors;  //一个线程对应一个subReactor，只在该线程内处理该TCP连接的事件循环
    ThreadPool* thpool;
    std::function<void(Connection *)> onConnectCallback;
public:
    explicit Server(EventLoop*);
    ~Server();
    void newConnection(Socket *sock);
    void deleteConnection(Socket *sock);
    void onConnect(std::function<void(Connection *)> fn);
};


