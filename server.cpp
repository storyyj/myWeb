#include "EventLoop.h"
#include "Server.h"
#include "Buffer.h"
#include "Socket.h"
#include "Connection.h"
#include <iostream>
#include <cstring>

int main()
{
    EventLoop *loop=new EventLoop();
    Server *server=new Server(loop);
    server->onConnect([](Connection *conn) {
        conn->Read();
        //read完后需要先判断客户端的状态
        if(conn->GetState()==Connection::State::Closed)
        {
            conn->Close();
            return;
        }
        std::cout<<"Message from client"<<conn->GetSocket()->getFd() <<":"<<conn->ReadBuffer()<<std::endl;
        int len=conn->GetReadBuffer()->Size();
        char mybuf[len+1];  //这里的buf开辟的空间要比readBuffer的大小+1，拿来放\0结束符，否则结尾处会出现乱码
        memset(mybuf,0,sizeof(mybuf));
        for(int i=0;i<len;i++)
        {
            mybuf[i]=toupper(conn->GetReadBuffer()->c_str()[i]);
        }
        printf("writeBuffer:%s is OK\n",mybuf);
        conn->SetSendBuffer(mybuf);
        conn->Write();
    });
    loop->loop();
    delete server;
    delete loop;
    return 0;
}