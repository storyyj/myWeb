#pragma once
#include<sys/epoll.h>
#include<functional>
#include<memory>  //管理动态内存，比如智能指针shared_ptr
#include<string>
#include<unordered_map>

class Socket;
class EventLoop;
class Channel
{
private:
    /* data */
    EventLoop* loop;
    int fd;
    uint32_t events;  //设置所要监听的事件
    uint32_t revents;  //用来接收由epoll返回的ready events
    bool inEpoll;
    //所有返回值是void，传入参数为空的可调用对象均可作为回调函数
    std::function<void()> readCallback;
    std::function<void()> writeCallback;
public:
    Channel(EventLoop *_loop,int _fd);
    ~Channel();
    void enableReading();
    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void setInEpoll(bool _in=true);
    void useET();
    void setEvents(uint32_t);
    void setRevents(uint32_t);
    void handleEvent();
    void setReadCallback(std::function<void()>);
    void setWriteCallback(std::function<void()>);
};

