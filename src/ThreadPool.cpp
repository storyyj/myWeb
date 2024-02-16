#include "ThreadPool.h"

ThreadPool::ThreadPool(int size) : stop(false)
{
    for(int i=0;i<size;++i)
    {
        //创建一个线程并将该线程放进threads队列中
        //thread(Fn&& fn, Args&&… args)创建一个线程，以args为参数，执行fn函数，这里传入的fn函数是一个lambda表达式
        threads.emplace_back(std::thread([this]()
        {
            while(true)
            {
                //创建一个task变量用来接收tasks任务队列的第一个任务
                std::function<void()> task;
                //这里用大括号括起来是为了让unique_lock出了当前作用域后可以自动解锁
                {
                    std::unique_lock<std::mutex> lock(tasks_mtx);
                    cv.wait(lock,[this](){
                        return stop||!tasks.empty();  //当任务列表不为空时该lambda函数返回1
                    });
                    if(stop&&tasks.empty()) return;
                    task=tasks.front();
                    tasks.pop();
                }
                //执行该任务，也就是创建该线程时直接执行task任务
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop=true;
    }
    cv.notify_all();
    for(std::thread &th:threads)
    {
        if(th.joinable())
        {
            th.join();
        }
    }
}