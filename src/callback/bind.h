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
*/

template <typename F, typename...Args>
struct CallbackTypeTraits;

template <typename F, typename...Args>
struct CallbackTypeTraits<F, TypeList<Args...>>
{
    using Type = Callback<typename function_traits<F>::return_type(Args...)>;
};


template <bool IsMemberMethod, typename Runnable, typename Runner, typename BoundArgTuple, typename...Args>
struct StorageTypeTraits;

template <bool IsMemberMethod, typename Runnable, typename Runner, typename BoundArgTuple, typename...Args>
struct StorageTypeTraits<IsMemberMethod, Runnable, Runner, BoundArgTuple, TypeList<Args...>>
{
    using StorageType = BindStorage<IsMemberMethod,
                                    typename function_traits<Runnable>::return_type,
									Runnable,
                                    Runner,
									BoundArgTuple>;
};

// 提取绑定的参数类型
// UnboundTypeTraits<>::Type 是 Types<...> 类型
template <typename F, typename... Args>
struct CallbackParamTraits
{
private:

	static constexpr bool IsMemberMethod = std::is_member_function_pointer<F>::value;

	static constexpr std::size_t BoundSize = RunnerTraits<IsMemberMethod, Args...>::BoundSize;

	// 未绑定的参数个数 = 函数参数个数 - 绑定的参数个数 - 成员函数指针参数
	static constexpr std::size_t UnBoundTypeSize =
		function_traits<F>::arity - BoundSize;
public:

	using UnboundTypeList = typename TypeListTraits<UnBoundTypeSize, typename function_traits<F>::tuple_type>::type;

	using RunnerType = typename RunnerTraits<IsMemberMethod, Args...>::RunnerType;

	using BoundArgTuple = typename RunnerTraits<IsMemberMethod, Args...>::RunnableTuple;
};

template<typename T, typename... Args>
typename CallbackTypeTraits<typename std::decay<T>::type,
	typename CallbackParamTraits<typename std::decay<T>::type, Args...>::UnboundTypeList >::Type
Bind(T func, Args... args)
{
	typedef typename std::decay<T>::type Runnable;

	typedef typename CallbackParamTraits<Runnable, Args...>::UnboundTypeList UnboundTypeList;
	typedef typename CallbackParamTraits<Runnable, Args...>::RunnerType RunnerType;
	typedef typename CallbackParamTraits<Runnable, Args...>::BoundArgTuple BoundArgTuple;
	typedef typename CallbackTypeTraits<Runnable, UnboundTypeList>::Type CallbackType;
	
	constexpr bool is_member_call = std::is_member_function_pointer<Runnable>::value;

	typedef typename StorageTypeTraits<
		is_member_call,
		Runnable,
		RunnerType,
		BoundArgTuple,
		UnboundTypeList >::StorageType StorageType;

	return CallbackType(new StorageType(func, args...));
};

typedef Callback<void(void)> Closure;

#endif /* __BIND_H__ */
