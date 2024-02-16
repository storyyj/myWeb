//专门抽象出一个Acceptor类用来处理新客户端到来的连接事件
#pragma once
#include <functional>

class EventLoop;
class Socket;
class Channel;
class Acceptor
{
private:
    /* data */
    EventLoop *loop;
    Socket *sock;
    Channel *acceptChannel;
    std::function<void(Socket*)> newConnectionCallback;
public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();
    void acceptConnection();
    void setNewConnectionCallback(std::function<void(Socket*)>);
};


