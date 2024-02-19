#pragma once
#include <functional>
class Epoller;
class Channel;

class EventLoop
{
private:
    /* data */
    Epoller *ep;
    bool quit;
public:
    EventLoop();
    ~EventLoop();
    void loop();
    void updateChannel(Channel*);
    void addThread(std::function<void()>);
};


