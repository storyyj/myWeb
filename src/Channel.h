#pragma once
#include<sys/epoll.h>
#include<functional>
#include<memory>  //管理动态内存，比如智能指针shared_ptr
#include<string>
#include<unordered_map>


class EventLoop;
class Channel
{
private:
    /* data */
    EventLoop* loop;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool inEpoll;
    //所有返回值是void，传入参数为空的可调用对象均可作为回调函数
    std::function<void()> callback;
public:
    Channel(EventLoop *_loop,int _fd);
    ~Channel();
    void enableReading();
    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void setInEpoll();
    void setEvents(uint32_t);
    void setRevents(uint32_t);
    void handleEvent();
    void setCallback(std::function<void()>);
};

