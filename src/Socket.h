#pragma once
//将socket fd的一系列操作封装成一个类
//包括bind，listen，accept，getFd获取自身fd，setnonblocking设置为非阻塞fd

class InetAddress;
class Socket
{
private:
    /* data */
    int fd;
public:
    Socket(int);
    Socket();
    ~Socket();
    void Bind(InetAddress*);
    void Listen();
    void SetnonBlocking();
    int Accept(InetAddress*);
    int getFd();

};


