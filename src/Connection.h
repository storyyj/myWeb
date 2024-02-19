#pragma once
#include <functional>
#include "Macros.h"

class EventLoop;
class Socket;
class Channel;
class Buffer;

class Connection
{
public:
    enum State{
        Invalid=1,
        Handshaking,
        Connected,
        Closed,
        Failed,
    };
    Connection(EventLoop *_loop,Socket *_sock);
    ~Connection();
    DISALLOW_COPY_AND_MOVE(Connection);
    void Read();
    void Write();
    
    //删除TCP连接的操作是由socket来执行的，所以回调函数的参数传入的是Socket*
    void setDeleteConnectionCallback(std::function<void(Socket*)> const &_cb);
    //而TCP连接上发生事件后的执行是由TCP连接自己执行的，中间加一些业务逻辑而已，所以这里传入的参数是Connection*
    void setOnConnectCallback(std::function<void(Connection*)> const &_cb);
    State GetState();
    void Close();
    //将经过业务逻辑运算后的数据放到writeBuffer中
    void SetSendBuffer(const char *str);
    //下面的这四个函数都是为了获取缓冲区中的数据，一个是返回Buffer类的指针，一个是返回char*指针
    Buffer *GetReadBuffer();
    const char *ReadBuffer();
    Buffer *GetSendBuffer();
    const char *SendBuffer();
    //将输入流cin的信息放到writeBuffer中，这个函数是给client使用的
    void GetlineSendBuffer();
    Socket *GetSocket();

    void OnConnect(std::function<void()> fn);

private:
    /* data */
    EventLoop *loop;
    Socket *sock;
    Channel *connectChannel{nullptr};
    std::function<void(Socket*)> deleteConnectionCallback;
    std::function<void(Connection*)> onConnectCallback;
    Buffer* readBuffer{nullptr};
    Buffer* writeBuffer{nullptr};
    State state{State::Invalid};
    void ReadNonBlocking();
    void WriteNonBlocking();
    void ReadBlocking();
    void WriteBlocking();
};


