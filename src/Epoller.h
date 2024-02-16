#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel;
class Epoller
{
private:
    /* data */
    int epfd;  //epoll红黑树的根节点
    epoll_event *revents;  //保存epoll_wait返回的发生的事件
public:
    Epoller();
    ~Epoller();
    void addFd(int fd,uint32_t op);
    void updateChannel(Channel*);
    //poll函数用来返回一个装着指向Channel类的指针vector数组，用来管理channel类
    std::vector<Channel*> poll(int timeout=-1);
};

