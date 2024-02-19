#include "Server.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "Connection.h"
#include <functional>
#include <string.h>
#include <unistd.h>
#include "ThreadPool.h"
#include "EventLoop.h"
#define READ_BUFFER 1024

Server::Server(EventLoop* _loop) : mainReactor(_loop),acceptor(nullptr),thpool(nullptr)
{
    acceptor=new Acceptor(mainReactor);
    //注意这里的bind是std空间中的bind，用来绑定回调函数用的，而不是全局空间中的::bind则是用来将ip地址绑定到socket上
    //std::bind将一个可调用对象和后面的传入参数（可用占位符替代）绑定成一个仿函数，该仿函数可以用std::function接收
    //因为类成员函数有一个隐含参数this，因此使用类成员（非静态成员)函数则需要传递this指针作为第一个参数
    //若在外部调用时，则将类实例（对象）本身的地址（&取地址）作为this指针传入
    std::function<void(Socket*)> cb=std::bind(&Server::newConnection,this,std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);

    int size=std::thread::hardware_concurrency();  //返回硬件支持的并发线程数
    thpool=new ThreadPool(size);
    //线程池有多少个线程就创建多少个subReactor
    for(int i=0;i<size;++i)
    {
        subReactors.push_back(new EventLoop());
    }
    for(int i=0;i<size;++i)
    {
        std::function<void()> sub_loop=std::bind(&EventLoop::loop,subReactors[i]);
        thpool->add(std::move(sub_loop));  //给每一个线程都添加一个loop回调函数
    }
}

Server::~Server()
{
    delete acceptor;
    delete thpool;
}


void Server::newConnection(Socket *sock)
{
    if(sock->getFd()!=-1)
    {
        int random=sock->getFd()%subReactors.size();
        Connection *conn=new Connection(subReactors[random],sock);
        std::function<void(Socket*)> cb=std::bind(&Server::deleteConnection,this,std::placeholders::_1);
        conn->setDeleteConnectionCallback(cb);
        conn->setOnConnectCallback(onConnectCallback);
        connectionsMap[sock->getFd()]=conn;
    }

}

void::Server::deleteConnection(Socket *sock)
{
    int sockfd=sock->getFd();
    if(sockfd!=-1)
    {
        if(connectionsMap.find(sockfd)!=connectionsMap.end())
        {
            Connection *conn=connectionsMap[sockfd];
            connectionsMap.erase(sock->getFd());
            delete conn;
            conn=nullptr;
        }
    }
}

void Server::onConnect(std::function<void(Connection *)> fn)
{
    onConnectCallback=std::move(fn);
}