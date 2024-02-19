#pragma once
#include <string>

class Buffer
{
private:
    /* data */
    std::string buf;
public:
    Buffer();
    ~Buffer();

    void Append(const char* _str,int size);
    ssize_t Size();
    const char* c_str();
    void Clear();
    void getLine();
    void setBuf(const char*);
};

