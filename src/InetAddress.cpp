#include "InetAddress.h"
#include<string.h>

//第一个冒号的后边相当于直接用初始化列表的方式用sizeof(addr)来初始化addr_len，但其实放到函数体内再赋值也可以
InetAddress::InetAddress():addr_len(sizeof(addr))
{
    bzero(&addr,addr_len);
}
InetAddress::InetAddress(const char* ip,uint16_t port):addr_len(sizeof(addr))
{
    bzero(&addr,addr_len);
    addr.sin_family=AF_INET;
    /*in_addr_t inet_addr (const char *__cp)的作用是将点分十进制字符串的ipv4地址转换为网络字节序整数表示的ipv4地址
    inet_addr的返回值类型in_addr_t，sockaddr_in.sin_addr.s_addr的类型刚好也是in_addr，因此要用s_addr来接收
    失败时返回INADDR_NONE
    inet_aton (const char *__cp, struct in_addr *__inp)，
    该函数的功能和inet_addr一样，不过他是将结果保存到inp指针所指向的结构体中
    成功返回1，失败返回0*/
    addr.sin_addr.s_addr=inet_addr(ip);
    addr.sin_port=htons(port);
    addr_len=sizeof(addr);
}

InetAddress::~InetAddress()
{
}

sockaddr_in InetAddress::getAddr()
{
    return addr;
}

socklen_t InetAddress::getAddr_len()
{
    return addr_len;
}