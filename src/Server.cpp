#include "Channel.h"
#include "Server.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "Connection.h"
#include <functional>
#include <string.h>
#include <unistd.h>
#define READ_BUFFER 1024

Server::Server(EventLoop* _loop) : loop(_loop),acceptor(nullptr)
{
    acceptor=new Acceptor(loop);
    //注意这里的bind是std空间中的bind，用来绑定回调函数用的，而不是全局空间中的::bind则是用来将ip地址绑定到socket上
    //std::bind将一个可调用对象和后面的传入参数（可用占位符替代）绑定成一个仿函数，该仿函数可以用std::function接收
    //因为类成员函数有一个隐含参数this，因此使用类成员（非静态成员)函数则需要传递this指针作为第一个参数
    //若在外部调用时，则将类实例（对象）本身的地址（&取地址）作为this指针传入
    std::function<void(Socket*)> cb=std::bind(&Server::newConnection,this,std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);
}

Server::~Server()
{
    delete acceptor;
}

void Server::handleReadEvent(int sockfd)
{

}
void Server::newConnection(Socket *sock)
{
    Connection *conn=new Connection(loop,sock);
    std::function<void(Socket*)> cb=std::bind(&Server::deleteConnection,this,std::placeholders::_1);
    conn->setDeleteConnectionCallback(cb);
    connectionsMap[sock->getFd()]=conn;
}

void::Server::deleteConnection(Socket *sock)
{
    Connection *conn=connectionsMap[sock->getFd()];
    connectionsMap.erase(sock->getFd());
    delete conn;
}