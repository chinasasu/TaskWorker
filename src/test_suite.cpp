//#include "gtest/gtest.h"
#include "callback/bind.h"
#include "base/string_helper.h"
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>

//int main(int argc, char** argv)
//{
//	testing::InitGoogleTest(&argc, argv);
//
//	return RUN_ALL_TESTS();
//}

std::mutex vMutex;
std::condition_variable cv;
bool ready = false;
bool processed = false;


class Foo
{
public:
    int Func();
    void Func1(int a){}
    void Func2(int a, char b){}
};


void Sum(int a, int b, int* outSumValue)
{
    *outSumValue = a + b;
}

void SumDemo(int a, char s, int* c)
{
    
}

void worker_thread()
{
    std::unique_lock<std::mutex> oo(vMutex);
    cv.wait(oo);
    std::this_thread::sleep_for(std::chrono::seconds(10));

//    
//    std::unique_lock<std::mutex> lk(vMutex);
//    cv.wait(lk, []{return ready;});
}

void work_thread2()
{
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::lock_guard<std::mutex> lk(vMutex);
    ready = true;
    cv.notify_all();
}

bool TestBool()
{
    return true;
}

int main(int argc, char** argv)
{
    //testing::InitGoogleTest(&argc, argv);
    int sum = 0;
    //Closure cb0 = Bind(&Sum, 32, 16, &sum);
    

    ////////
    
    
    //BindDemo(&SumDemo, 0,std::placeholders::_1,0);
    
    //BindDemo(&SumDemo, 3,'d',std::placeholders::_1);
    
    //std::function<void(int*)> ff = std::bind(&SumDemo, 3,'d',std::placeholders::_1);
    
    ///////
    
    Callback<bool(void)> cb2 = Bind(&TestBool);
//    
    Callback<void(void)> cbbb =  Bind(&SumDemo, 3,'d',&sum);
   
    Callback<void(int,char)> cbbb2 =  Bind(&SumDemo, std::placeholders::_1, std::placeholders::_2,&sum);
    
    
    Foo foo;
    
    auto cl = Bind(&Foo::Func1, &foo,std::placeholders::_1);
    
    
    std::function<void(char)> df= std::bind(&Foo::Func2, &foo, 2, std::placeholders::_1);

    
     auto cbb3 =  Bind(&SumDemo, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
    
    //cbbb.Run(0);
    
    //std::function<void(int&,char)>a =  std::bind(&SumDemo, std::placeholders::_1, std::placeholders::_2,&sum);
    
    
    //Callback<bool(void)> cb2 = Bind(&TestBool);

    //cb0.Run();

//    std::string v = StringPrintf("%d lala ", sum);
//
//    Closure cb = Bind([&]{ sum = 100;});
//
//    cb.Run();
//    
//    std::thread worker(worker_thread);
//    std::thread worker2(work_thread2);
//
//    
//    worker.join();
//    worker2.join();
//    
    
    
    return 0;
}