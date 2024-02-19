#include "Acceptor.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"

Acceptor::Acceptor(EventLoop* _loop) : loop(_loop),sock(nullptr),acceptChannel(nullptr)
{
    sock=new Socket();
    //将启动服务器时创建一个监听Socket的任务抽象成一个acceptor类
    InetAddress *addr=new InetAddress("127.0.0.1", 1234);
    sock->Bind(addr);
    sock->Listen();
    // sock->SetnonBlocking();  acceptor设为阻塞式的IO比较好，客户端的TCP连接再设置为非阻塞式的
    acceptChannel=new Channel(loop,sock->getFd());
    //这里设置的是监听socket监听到新连接时的回调函数
    std::function<void()> cb=std::bind(&Acceptor::acceptConnection,this);
    acceptChannel->setReadCallback(cb);
    acceptChannel->enableReading();
    delete addr;
}

Acceptor::~Acceptor()
{
    delete sock;
    delete acceptChannel;
}

void Acceptor::acceptConnection()
{
    InetAddress *clnt_addr=new InetAddress();
    Socket *clnt_sock=new Socket(sock->Accept(clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n",clnt_sock->getFd(),inet_ntoa(clnt_addr->getAddr().sin_addr),ntohs(clnt_addr->getAddr().sin_port));
    clnt_sock->setNonBlocking();
    newConnectionCallback(clnt_sock);
    delete clnt_addr;
}

//这里则是为每个新TCP连接有读写事件发生时设置的处理逻辑回调函数
void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> _cb)
{
    newConnectionCallback=_cb;
}