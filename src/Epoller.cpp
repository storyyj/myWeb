#include "Epoller.h"
#include "util.h"
#include "Channel.h"
#include <unistd.h>
#include <string.h>

#define MAXEVENTS 1024

Epoller::Epoller():epfd(-1),revents(nullptr)
{
    //epoll_create1相对于epoll_create只是丢掉了没用的size参数而已，功能一样，加了个flag参数，一般传0即可
    epfd=epoll_create1(0);
    errif(epfd==-1,"epoll create error");
    revents=new epoll_event[MAXEVENTS];
    //这里创建的events是一个epoll_event数组，events是指向该数组头的指针，因此要获取他的大小要sizeof(*events)解引用;
    bzero(revents,sizeof(*revents)*MAXEVENTS);
}

Epoller::~Epoller()
{
    if(epfd!=-1)
    {
        close(epfd);
        epfd=-1;
    }
    delete [] revents;
}

//之前是在Epoller对象里实现fd的添加，现在转到Channel中，Epoll对象只负责管理Channel对象
// void Epoller::addFd(int fd,uint32_t op)
// {
//     struct epoll_event ev;
//     bzero(&ev,sizeof(ev));
//     ev.data.fd=fd;
//     ev.events=op;
//     errif(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev)==-1,"epoll add event error");
// }

//调用epoll_wait，将发生的事件所对应的fd绑定到channel对象，并将channel对象放进vector数组中准备处理
std::vector<Channel*> Epoller::poll(int timeout)
{
    std::vector<Channel*> activeChannels;
    int nfds=epoll_wait(epfd,revents,MAXEVENTS,timeout);
    errif(nfds==-1,"epoll wait error");
    for(int i=0;i<nfds;++i)
    {
        Channel* ch=(Channel*)revents[i].data.ptr;
        ch->setRevents(revents[i].events);
        activeChannels.push_back(ch);
    }
    return activeChannels;
}

void Epoller::updateChannel(Channel* channel)
{
    int fd=channel->getFd();
    struct epoll_event ev;
    bzero(&ev,sizeof(ev));
    ev.data.ptr=channel;
    ev.events=channel->getEvents();
    if(!channel->getInEpoll())
    {
        errif(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev)==-1,"epoll add error");
        channel->setInEpoll();
    }
    else
    {
        errif(epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev)==-1,"epoll modify error");
        
    }
}