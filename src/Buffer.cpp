#include "Buffer.h"
#include <string.h>
#include <iostream>


Buffer::Buffer() {}

Buffer::~Buffer() {}

//将数据存到buf中
void Buffer::Append(const char* _str,int _size)
{
    for(int i=0;i<_size;++i)
    {
        if(_str[i]=='\0') break;
        buf.push_back(_str[i]);
    }
}

ssize_t Buffer::Size()
{
    return buf.size();
}

//c_str()函数返回一个指向正规C字符串的指针常量, 内容与本string串相同。
//这是为了与c语言兼容，在c语言中没有string类型，故必须通过string类对象的成员函数c_str()把string 对象转换成c中的字符串样式。
const char* Buffer::c_str()
{
    return buf.c_str();
}

void Buffer::Clear()
{
    buf.clear();
}

void Buffer::getLine()
{
    buf.clear();
    //将输入流cin中的信息存到buf中
    std::getline(std::cin,buf);
}

void Buffer::setBuf(const char* _buf)
{
    buf.clear();
    buf.append(_buf);
}
