#pragma once
#include<arpa/inet.h>
//将地址转换封装成一个类，往构造函数中传入IP地址和port端口，直接绑定到sockaddr_in结构体上
class InetAddress
{
public:
    /* data */
    sockaddr_in addr;
    socklen_t addr_len;

public:
    InetAddress();
    InetAddress(const char* ip,uint16_t port);
    ~InetAddress();
    sockaddr_in getAddr();
    socklen_t getAddr_len();
};


