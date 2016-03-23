/* -------------------------------------------------------------------------
//     filename :	bind.h
//     author   :	cjzhou@tencent.cn
//     created  :	2016-3-10 20:24
//     purpose  :
//
// -----------------------------------------------------------------------*/
#ifndef __BIND_H__
#define __BIND_H__

#include "callback/bind_storage.h"
#include "callback/bind_callback.h"
#include <type_traits>
/*
Useage:
	
void Sum(int a, int b, int* outSumValue)
{
	*outSumValue = a + b;
}

class Foo
{
public:
	void DoSome() { // ... }
	void DoWith1Param(int a) { // ...}
}

// 
	// 对Sum函数做了一个闭包，对于值传参数，内部存储
	// 支持指针类型
	// 重要：不支持引用修改传递
	Closure cb = Bind(&Sum, 32, 16, &sum);
	
	// 这里是延迟调用，会执行Sum函数
	cb.Run();


	Bind支持的方法类型有：Lamba函数，成员方法，全局函数;
	调用者可以是裸指针、std的智能指针，std的弱引用指针（支持弱引用判断是否有效，无效时自动不执行回调）

a. 裸指针
	Foo foo;
	Closure cb = Bind(&Foo::DoSome, &foo);
	Closure cb1 = Bind(&Foo::DoWith1Param, &foo, 32);

b. 智能指针
	std::shared_ptr<Foo> sharedFoo(new Foo);
	Closure cb = Bind(&Foo::DoSome, sharedFoo);
	Closure cb1 = Bind(&Foo::DoWith1Param, sharedFoo, 32);

c. 弱指针
	std::shared_ptr<Foo> sharedFoo(new Foo);
	std::weak_ptr<Foo> weakFoo(sharedFoo);
	Closure cb = Bind(&Foo::DoSome, weakFoo);
	Closure cb1 = Bind(&Foo::DoWith1Param, weakFoo, 32);

	// 如果weakFoo所持有的指针已被释放，这里的调用将不会调用闭包的函数
	// *如果是用其它类型的闭包，则不能进行此类判断*
	weakFoo.Run();

*/

template<typename F, typename C, typename... Args,
	typename = std::enable_if_t < (std::is_member_function_pointer<F>::value) >>
	Callback<void(void)> Bind(F method, C runner, Args... args)
{
	return Callback<void(void)>(new BindStorage<IsWeakMethod<C>::value, C, Args...>(method, runner, args...));
}

template<typename F, typename... Args>
Callback<void(void)> Bind(F function, Args... args)
{
	return Callback<void(void)>(new BindStorage<false, void, void>(function, args...));
}

typedef Callback<void(void)> Closure;


#endif /* __BIND_H__ */