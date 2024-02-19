#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <fcntl.h>  //对fd进行操作，设置非阻塞fd
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdint.h>

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

void Socket::setNonBlocking()
{
    fcntl(fd,F_SETFL,fcntl(fd,F_GETFL)|O_NONBLOCK);
}

bool Socket::isNonBlocking()
{
    //若fd不是NONBLOCK的非阻塞IO，与O_NONBLOCK &后=0
    return ((fcntl(fd,F_GETFL)&O_NONBLOCK)!=0);
}

//接收一个新客户端连接，返回该客户端的fd
int Socket::Accept(InetAddress* InetAddr)
{
    //for server socket
    //如果该fd被设置为非阻塞模式
    int client_fd=-1;
    if(fcntl(fd,F_GETFL)&O_NONBLOCK)
    {
        while(true)
        {
            client_fd=accept(fd,(sockaddr*)&InetAddr->addr,&InetAddr->addr_len);
            if(client_fd==-1&&((errno==EAGAIN)||(errno==EWOULDBLOCK)))
            {
                // printf("no connection yet\n");
                continue;
            }
            else if(client_fd==-1)
            {
                errif(true,"socket accept error");
            }
            else{
                break;
            }
        }
    }
    else
    {
        client_fd=accept(fd,(sockaddr*)&InetAddr->addr,&InetAddr->addr_len);
        errif(client_fd==-1,"socket accept error");
    }
    return client_fd;
}

void Socket::Connect(InetAddress* InetAddr)
{
    //for client socket
    //给客户端socket传入一个存有服务器IP地址和端口的InetAddr对象，封装客户端连接到服务器的过程
    struct sockaddr_in addr=InetAddr->getAddr();
    if(fcntl(fd,F_GETFL)&O_NONBLOCK)
    {
        while(true)
        {
            int ret=connect(fd,(sockaddr*)&addr,sizeof(addr));  //accept的addr_len需要用指针的形式传入，而connect不需要
            if(ret==0)
            {
                break;
            }
            else if(ret==-1&&(errno==EINPROGRESS))
            {
                continue;
            }
            else if(ret==-1)
            {
                errif(true,"socket connect error");
            }
        }
    }
    else{
        errif(connect(fd,(sockaddr*)&addr,sizeof(addr))==-1,"socket connect error");
    }
}
void Socket::Connect(const char *ip, uint16_t port) {
    InetAddress *addr = new InetAddress(ip, port);
    Connect(addr);
    delete addr;
}

int Socket::getFd()
{
    //隐藏fd成员变量，只开放函数接口用以获取fd，避免fd被误操作
    return fd;
}
