#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include "Buffer.h"
#include "util.h"
#include "Macros.h"
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <cassert>
#include <utility>
#include <sys/socket.h>

#define READ_BUFFER 1024

Connection::Connection(EventLoop *_loop,Socket *_sock) : loop(_loop),sock(_sock)
{
    //new返回的是一个指向该对象的指针，因此下面对于该类成员函数的使用要用->
    //如果是服务器会分发一个eventloop进行循环监听，就要封装一个Channel
    //如果是客户端不需要用到eventloop，同样也就不需要用到Channel了
    if(loop!=nullptr)
    {
        connectChannel=new Channel(loop,sock->getFd());
        connectChannel->enableReading();
        connectChannel->useET();  
    }
    writeBuffer=new Buffer();
    readBuffer=new Buffer();
    state=State::Connected;
}

Connection::~Connection()
{
    //因为这里的loop是共用的，所以析构函数里不需要delete loop，只有server的析构函数才需要delete loop
    if(loop!=nullptr)
    {
        delete connectChannel;
    }
    delete sock;
    delete readBuffer;
    delete writeBuffer;
}

void Connection::Read()
{
    ASSERT(state==State::Connected,"connection state is disconnected");
    readBuffer->Clear();
    if(sock->isNonBlocking())
    {
        ReadNonBlocking();
    }
    else
    {
        ReadBlocking();
    }
}

void Connection::Write()
{
    ASSERT(state==State::Connected,"connection state is disconnected");
    if(sock->isNonBlocking())
    {
        WriteNonBlocking();
    }
    else
    {
        WriteBlocking();
    }
    writeBuffer->Clear();
}

void Connection::ReadNonBlocking()
{
    int sockfd=sock->getFd();
    char buf[1024];
    while(true)
    {
        memset(buf,0,sizeof(buf));
        ssize_t bytes_read=read(sockfd,buf,sizeof(buf));
        if (bytes_read > 0) {
            readBuffer->Append(buf, bytes_read);
        } else if (bytes_read == -1 && errno == EINTR) {  // 程序正常中断、继续读取
            printf("continue reading\n");
            continue;
        } else if (bytes_read == -1 &&((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
            break;
        } else if (bytes_read == 0) {  // EOF，客户端断开连接
            printf("read EOF, client fd %d disconnected\n", sockfd);
            state = State::Closed;
            break;
        } else {
            printf("Other error on client fd %d\n", sockfd);
            state = State::Closed;
            break;
        }
    }
}

void Connection::WriteNonBlocking() {
    int sockfd = sock->getFd();
    char buf[writeBuffer->Size()];
    memcpy(buf, writeBuffer->c_str(), writeBuffer->Size());
    int data_size = writeBuffer->Size();
    int data_left = data_size;
    while (data_left > 0) {
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
        if (bytes_write == -1 && errno == EINTR) {
            printf("continue writing\n");
            continue;
        }
        if (bytes_write == -1 && errno == EAGAIN) {
            break;
        }
        if (bytes_write == -1) {
        printf("Other error on client fd %d\n", sockfd);
            state = State::Closed;
            break;
        }
        data_left -= bytes_write;
    }
}

//阻塞读和阻塞写只供client使用，服务器的读写都是用的非阻塞IO
//尽管监听IO设置为阻塞也是在accept类中设置，和connection类无关
void Connection::ReadBlocking()
{
    int sockfd=sock->getFd();
    //获取该socket的接收缓冲区的大小
    //因为在阻塞IO下，如果一次read()没有将所有数据读取完毕，它不会自动再次调用来读取剩下的数据。
    unsigned int rcv_size=0;
    socklen_t len=sizeof(rcv_size);
    getsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&rcv_size,&len);
    char buf[rcv_size];
    ssize_t bytes_read=read(sockfd,buf,sizeof(buf));  //只调用一次read
    if(bytes_read>0)
    {
        readBuffer->Append(buf,bytes_read);
    }
    else if(bytes_read==0)
    {
        printf("read EOF,blocking client fd %d disconnected\n",sockfd);
        state=State::Closed;
    }else if(bytes_read==-1)
    {
        printf("Other error on blocking client fd %d\n",sockfd);
        state=State::Closed;
    }
}

void Connection::WriteBlocking()
{
    int sockfd=sock->getFd();
    ssize_t bytes_write=write(sockfd,writeBuffer->c_str(),writeBuffer->Size());
    if(bytes_write==-1)
    {
        printf("Other error on blocking client fd %d\n",sockfd);
        state=State::Closed;
    }
}
void Connection::Close()
{
    deleteConnectionCallback(sock);
}
Connection::State Connection::GetState() { return state; }
void Connection::SetSendBuffer(const char* str) { writeBuffer->setBuf(str); }
Buffer *Connection::GetReadBuffer() { return readBuffer; }
const char *Connection::ReadBuffer() { return readBuffer->c_str(); }
Buffer *Connection::GetSendBuffer() { return writeBuffer; }
const char *Connection::SendBuffer() { return writeBuffer->c_str(); }

void Connection::setDeleteConnectionCallback(std::function<void(Socket *)> const &_cb)
{
    deleteConnectionCallback=_cb;
}

void Connection::setOnConnectCallback(std::function<void(Connection *)> const &_cb)
{
    onConnectCallback=_cb;
    connectChannel->setReadCallback([this]() {onConnectCallback(this); });
}

void Connection::GetlineSendBuffer()
{
    writeBuffer->getLine();
}

Socket *Connection::GetSocket()
{
    return sock;
}