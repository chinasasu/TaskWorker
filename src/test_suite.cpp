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
    int Func(){ return 0;}
    void Func1(int a){}
    void Func2(int a, char b){}
    void Func3(std::string a, int b)
    {
        int afda;
    }
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

bool TestBool2(int a, std::string b)
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
    
    std::shared_ptr<Foo> sharedFoo(new Foo);
    std::weak_ptr<Foo> weakFoo(sharedFoo);

    std::function<void(int)> wwf = std::bind(&Foo::Func1, sharedFoo, std::placeholders::_1);
    
    
    std::function<void(void)> ssss = std::bind([]{ int a =10;});
    
    
    
    //wwf(3);
    
    
   // Callback<bool(void)> cb2 = Bind(&TestBool);
//    
//    Callback<void(void)> cbbb =  Bind(&SumDemo, 3,'d',&sum);
   
    //Callback<void(int,char)> cbbb2 =  Bind(&SumDemo, std::placeholders::_1, std::placeholders::_2,&sum);
    
    
    Foo foo;
    
    auto c0 = Bind(&Foo::Func, &foo);
    auto cl = Bind(&Foo::Func1, &foo);
    auto c2 = Bind(&Foo::Func1, weakFoo, 1);
    auto c3 = Bind(&Foo::Func3, &foo);
    auto c33 = Bind(&Foo::Func3, &foo, "dfs");
    auto c4 = Bind(&Foo::Func3, weakFoo);
    
    c4.Run("a", 3);
    
    auto c5 = Bind(&TestBool2, 1);
    
    c5.Run("afd");
    
    int aaaa = 0;
    auto c6 = Bind([&]() { return "akj";});
    
    auto c7 = Bind([]() { return "akj";});
    
    //c6.Run((lambda) &args, <#int args#>);
    
    //c5.Run("d");
    //auto c3332 = Bind([]{ int a =10;});
    
    //cl.Run(2);
    
   // std::function<void(char)> df= std::bind(&Foo::Func2, &foo, 2, std::placeholders::_1);

    
     //auto cbb3 =  Bind(&SumDemo, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
    
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
