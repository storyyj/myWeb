/*类的声明以及定义分开写是为了避免将定义放在头文件，
那么当多个源文件使用#include命令包含此类的头文件便会在链接阶段出现“multiple definition”链接错误！*/
#pragma once
#include <mutex>
#include <queue>
#include <functional>
#include <future>
#include <thread>
#include <utility>
#include <vector>
#include <condition_variable>

class ThreadPool
{
private:
    /* data */
    std::vector<std::thread> threads;  //存放线程池里的消费者：工作线程
    std::queue<std::function<void()>> tasks;  //任务队列，存放监听的事件所对应的回调函数，生产者生产商品的过程相当于往任务队列中加回调函数
    std::mutex tasks_mtx;  //涉及到多线程，肯定需要线程同步，用mutex互斥锁的cv条件变量来实现线程同步
    std::condition_variable cv;
    bool stop;  //判断线程池是否关闭
public:
    ThreadPool(int);  //将线程池初始化为n个线程
    ~ThreadPool();
    //void add(std::function<void()>);  //这种添加任务的函数只能添加返回值为void且没有参数传入的回调函数，泛化性不强

    //采用可变模板参数来构造add函数
    template<class F,class... Args>
    auto add(F&&f,Args&&... args)
    ->std::future<typename std::result_of<F(Args...)>::type>;
};

//因为C++编译器不支持模板的分离编译，所以add模板的声明和定义都在头文件中一块儿给出
template<class F,class... Args>
auto ThreadPool::add(F&&f,Args&&... args)->std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type=typename std::result_of<F(Args...)>::type;
    auto task=std::make_shared< std::packaged_task<return_type()> >(
        std::bind(std::forward<F>(f),std::forward<Args>(args)...)
    );
    std::future<return_type> res=task->get_future();
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        if(stop)
        {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        /*传入的是一个lambda表达式，[task]指在当前函数体内部的捕获列表为task变量，()传入参数可以为空，返回类型也可以为空，
        {(*task)();} 大括号内部表示的是函数体，函数体为函数名task(函数名是一个指针)所指向的代码，注意；分号不要漏*/
        tasks.emplace([task](){(*task)();});  
    }
    cv.notify_one();
    return res;
}






