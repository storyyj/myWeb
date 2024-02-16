#include "Epoller.h"
#include "Channel.h"
#include "EventLoop.h"
#include <vector>

EventLoop::EventLoop() :ep(nullptr),quit(false)
{
    ep=new Epoller();
}

EventLoop::~EventLoop()
{
    delete ep;
}

void EventLoop::loop()
{
    while(!quit)
    {
        std::vector<Channel*> chs;
        chs=ep->poll();
        for(auto it=chs.begin();it!=chs.end();it++)
        {
            (*it)->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel* ch)
{
    ep->updateChannel(ch);
}