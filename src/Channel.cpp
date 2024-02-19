#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include <unistd.h>
#include <sys/epoll.h>

Channel::Channel(EventLoop* _loop,int _fd) : loop(_loop),fd(_fd),events(0),revents(0),inEpoll(false)
{
}

Channel::~Channel()
{
    if(fd!=-1)
    {
        close(fd);
        fd=-1;
    }
}

//为每个channel封装的不同事件设置不同的回调函数
void Channel::handleEvent()
{
    if(revents&(EPOLLIN||EPOLLPRI))
    {
        readCallback();  //如果epoll返回的revent是读事件则触发读回调，EPOLLPRI表示由带外数据触发
    }
    if(revents&(EPOLLOUT))
    {
        writeCallback();
    }
}

void Channel::enableReading()
{
    events=EPOLLIN||EPOLLPRI;
    loop->updateChannel(this);
}

void Channel::useET()
{
    events|=EPOLLET;
    loop->updateChannel(this);
}

int Channel::getFd()
{
    return fd;
}

uint32_t Channel::getEvents()
{
    return events;
}

uint32_t Channel::getRevents()
{
    return revents;
}

bool Channel::getInEpoll()
{
    return inEpoll;
}

void Channel::setInEpoll(bool _in)
{
    inEpoll=true;
}

void Channel::setEvents(uint32_t _ev){
    events = _ev;
}

void Channel::setRevents(uint32_t _ev){
    revents = _ev;
}

void Channel::setReadCallback(std::function<void()> _cb){
    readCallback = _cb;
}

void Channel::setWriteCallback(std::function<void()> _cb)
{
    writeCallback=_cb;
}