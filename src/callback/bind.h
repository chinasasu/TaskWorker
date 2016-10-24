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
#include <memory>
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

// typename RetType = typename std::result_of<F>::type
//template<typename F, typename C, typename... Args,
//	typename = std::enable_if_t < (std::is_member_function_pointer<F>::value) >>
//	Callback<void(void)> Bind(F method, C runner, Args... args)
//{
//	return Callback<void(void)>(new BindStorage<IsWeakMethod<C>::value, void, C, Args...>(method, runner, args...));
//}
//
//template<typename F, typename... Args>
//Callback<void(void)> Bind(F function, Args... args)
//{
//    //std::function<void (int)>::argument_type a;
//	return Callback<void(void)>(new BindStorage<false,void, void, void>(function, args...));
//}

template <bool IsWeakCall, typename F, typename Runner, typename... UnboundArgs>
struct CallbackTraits;

template <bool IsWeakCall, typename F, typename Runner, typename... UnboundArgs>
struct CallbackTraits<IsWeakCall, F, Runner, TypeList<UnboundArgs...>>
{
	using ReturnType = typename function_traits<F>::return_type;

	using Type = Callback<ReturnType(UnboundArgs...)>;

	using StorageType = BindStorage<IsWeakCall, ReturnType, Runner, UnboundArgs... >;
};

template<typename F, typename... Args>
decltype(auto) Bind(F&& function, const Args&... args)
{
	// bind的参数个数，如果是成员函数，不计算第一个参数
    using BoundArgsNumType = BoundArgsSize<std::is_member_function_pointer<F>::value, Args...> ;
    
    // 未绑定的参数个数 = 函数参数个数 - 绑定的参数个数 - 成员函数指针本身
	static constexpr std::size_t UnBoundTypeSize = 
		function_traits<F>::arity - BoundArgsNumType::size - (std::is_member_function_pointer<F>::value ? 1 : 0);

	// 未绑定的参数类型
    using UnboundTypeList = typename UnboundTypeTraits<UnBoundTypeSize,	typename function_traits<F>::tuple_type>::type;
    
    using IsWeakCall = IsWeakMethod<std::is_member_function_pointer<F>::value, Args...>;
    using MethodRunner = typename MethodRunnerTraits<std::is_member_function_pointer<F>::value, Args...>::type;
    
	// callback类型
    using CallbackType = typename CallbackTraits<IsWeakCall::value, F, MethodRunner, UnboundTypeList>::Type;
	
	// 存储类型
    using StorageType = typename CallbackTraits<IsWeakCall::value, F, MethodRunner, UnboundTypeList>::StorageType;
    
    return CallbackType(new StorageType(function, args...));
}

typedef Callback<void(void)> Closure;


#endif /* __BIND_H__ */
