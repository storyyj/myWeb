#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <fcntl.h>  //对fd进行操作，设置非阻塞fd
#include <unistd.h>
#include <sys/socket.h>

Socket::Socket(int _fd):fd(_fd)
{
    errif(fd==-1,"socket creat error");
}
Socket::Socket():fd(-1)
{
    fd=socket(AF_INET,SOCK_STREAM,0);
    errif(fd==-1,"socket creat error");
}
Socket::~Socket()
{
    if(fd!=-1)
    {
        close(fd);
        fd=-1;
    }
}

void Socket::Bind(InetAddress* InetAddr)
{
    //这里bind前面加两个::表示这里的bind函数是全局空间的bind，而不是Socket类中的bind，
    //我们可以直接将Socket类中的函数用大写表示以作区分，这样前面的::就可以省略了
    errif(::bind(fd,(sockaddr*)&InetAddr->addr,InetAddr->addr_len)==-1,"socket bind error");
}

void Socket::Listen()
{
    errif(::listen(fd,SOMAXCONN)==-1,"socket listen error");
}

void Socket::SetnonBlocking()
{
    fcntl(fd,F_SETFL,fcntl(fd,F_GETFL)|O_NONBLOCK);
}

//接收一个新客户端连接，返回该客户端的fd
int Socket::Accept(InetAddress* InetAddr)
{
    int client_fd=accept(fd,(sockaddr*)&InetAddr->addr,&InetAddr->addr_len);
    errif(client_fd==-1,"socket accept error");
    return client_fd;
}

int Socket::getFd()
{
    //隐藏fd成员变量，只开放函数接口用以获取fd，避免fd被误操作
    return fd;
}
