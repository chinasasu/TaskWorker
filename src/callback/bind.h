/* -------------------------------------------------------------------------
//     filename :	bind.h
//     author   :	cjzhou@tencent.cn
//     created  :	2016-3-10 20:24
//     purpose  :
//
// -----------------------------------------------------------------------*/
#ifndef __BIND_H__
#define __BIND_H__

#include <type_traits>
#include <memory>

#include "callback/bind_storage.h"
#include "callback/bind_callback.h"
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

template <typename F, typename...Args>
struct CallbackTypeTraits;

template <typename F, typename...Args>
struct CallbackTypeTraits<F, TypeList<Args...>>
{
    using Type = Callback<typename function_traits<F>::return_type(Args...)>;
};


template <bool IsWeakCall, typename Runner, typename F, typename...Args>
struct StorageTypeTraits;

template <bool IsWeakCall, typename Runner, typename F, typename...Args>
struct StorageTypeTraits<IsWeakCall, Runner, F, TypeList<Args...>>
{
    using StorageType = BindStorage<IsWeakCall,
                                    typename function_traits<F>::return_type,
                                    Runner,
                                    Args... >;
};

template<typename F>
decltype(auto) Bind(F&& function)
{
    typedef TypeList<> UnboundTypeList;
    typedef typename CallbackTypeTraits<F, UnboundTypeList>::Type CallbackType;

    typedef typename StorageTypeTraits<
    false,
    void,
    F,
    UnboundTypeList >::StorageType StorageType;

    return CallbackType(new StorageType(function));
}


template<typename F, typename... Args>
typename CallbackTypeTraits<F,
    typename CallbackParamTraits<F, Args...>::UnboundTypeList >::Type
Bind(F&& function, const Args&... args)
{
    typedef typename CallbackParamTraits<F, Args...>::UnboundTypeList UnboundTypeList;
    typedef typename CallbackParamTraits<F, Args...>::RunnerType RunnerType;
    typedef typename CallbackTypeTraits<F, UnboundTypeList>::Type CallbackType;
        
    constexpr bool is_weak_call = CallbackParamTraits<F, Args...>::IsWeakCall::value;

    typedef typename StorageTypeTraits<
                    is_weak_call,
                    RunnerType,
                    F,
                    UnboundTypeList >::StorageType StorageType;
    
    return CallbackType(new StorageType(function, args...));
}

typedef Callback<void(void)> Closure;

//template<typename F>
//decltype(auto) Bind(F&& function)
//{
//    // 未绑定的参数个数 = 函数参数个数 - 绑定的参数个数 - 成员函数指针本身
//    static constexpr std::size_t UnBoundTypeSize = function_traits<F>::arity;
//
//    // 未绑定的参数类型
//    using UnboundTypeList = typename UnboundTypeTraits<UnBoundTypeSize,	typename function_traits<F>::tuple_type>::type;
//
//    using IsWeakCall = IsWeakMethod<std::is_member_function_pointer<F>::value, Args...>;
//    using MethodRunner = typename MethodParamTraits<std::is_member_function_pointer<F>::value, Args...>::Runnertype;
//
//    // callback类型
//    using CallbackType = typename CallbackTraits<IsWeakCall::value, F, MethodRunner, UnboundTypeList>::Type;
//
//    // 存储类型
//    using StorageType = typename CallbackTraits<IsWeakCall::value, F, MethodRunner, UnboundTypeList>::StorageType;
//
//    return CallbackType(new StorageType(function, args...));
//
//
////    using ReturnType = typename function_traits<F>::return_type;
////
////    using UnboundTypeList = typename UnboundTypeTraits<0, typename function_traits<F>::tuple_type>::type;
////
////    using CallbackType = Callback<void()>;
////
////    using StorageType = BindStorage<false, void, void >;
////
////    return CallbackType(new StorageType(function));
//}


#endif /* __BIND_H__ */
