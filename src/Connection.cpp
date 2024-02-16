#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include <unistd.h>
#include <string.h>
#define READ_BUFFER 1024

Connection::Connection(EventLoop *_loop,Socket *_sock) : loop(_loop),sock(_sock),connectChannel(nullptr)
{
    connectChannel=new Channel(loop,sock->getFd());
    std::function<void()>cb =std::bind(&Connection::echo,this,sock->getFd());
    connectChannel->setCallback(cb);
    connectChannel->enableReading();
}

Connection::~Connection()
{
    //因为这里的loop是共用的，所以析构函数里不需要delete loop，只有server的析构函数才需要delete loop
    delete connectChannel;
    delete sock;
}

void Connection::echo(int sockfd)
{
    char buf[READ_BUFFER];
    while(true)
    {
        bzero(&buf,sizeof(buf));
        ssize_t bytes_read=read(sockfd,buf,sizeof(buf));
        if(bytes_read>0)  //正常读取
        {
            printf("message from client fd %d: %s\n",sockfd,buf);
            //将其转换成大写后返回
            for(int j=0;j<bytes_read;j++)
            {
                buf[j]=toupper(buf[j]);
            }
            write(sockfd,buf,sizeof(buf));
        }
        else if(bytes_read==-1&&errno==EINTR)  //客户端正常中断，继续读取
        {
            printf("continue reading\n");
            continue;
        }
        else if(bytes_read==0)  //EOF,客户端断开连接，调用deleteConnection的回调函数
        {
            printf("EOF,client fd %d disconnected\n",sockfd);
            close(sockfd);
            deleteConnectionCallback(sock);
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket *)> _cb)
{
    deleteConnectionCallback=_cb;
}