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


template <bool IsMethod, typename... Args>
struct IsWeakMethodEx : public std::false_type {};


template <typename T, typename... Args>
struct IsWeakMethodEx<true, std::weak_ptr<T>, Args...> : public std::true_type {};

template <typename T, typename... Args>
struct IsWeakMethodEx<true, std::weak_ptr<T>&, Args...> : public std::true_type {};

template <typename T, typename... Args>
struct IsWeakMethodEx<true, const std::weak_ptr<T>&, Args...> : public std::true_type {};


template <bool IsWeakCall, typename... Args>
struct RunnerType
{
    using type = void;
};

template <typename T, typename... Args>
struct RunnerType<true, T, Args...>
{
    using type = T;
};


template <bool IsMethod, typename... Args>
struct BoundArgsNum;

template<typename T, typename... Args>
struct BoundArgsNum<true, T, Args...>
{
    static constexpr std::size_t size = sizeof...(Args);
};


template<typename... Args>
struct BoundArgsNum<false, Args...>
{
    static constexpr std::size_t size = sizeof...(Args);
};


/*
 1. 分离绑定参数和运行时参数
 2. 生成std::function变量存储
 3. Run时绑定运行时参数
*/

// https://functionalcpp.wordpress.com/2013/08/05/function-traits/

template<class F>
struct function_traits;

// function pointer
template<class R, class... Args>
struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
{};

template<class R, class... Args>
struct function_traits<R(Args...)>
{
    using return_type = R;
    
    static constexpr std::size_t arity = sizeof...(Args);
    
    template <std::size_t N>
    struct argument
    {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
    };
    
    using tuple_type = std::tuple<Args...>;
};

// member function pointer
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...)> : public function_traits<R(C&,Args...)>
{};

template<class C, class R, class T, class... Args>
struct function_traits<R(C::*)(std::weak_ptr<T>,Args...)> : public function_traits<R(C&,T*, Args...)>
{};

// const member function pointer
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...) const> : public function_traits<R(C&,Args...)>
{};

// member object pointer
template<class C, class R>
struct function_traits<R(C::*)> : public function_traits<R(C&)>
{};


template <typename...>
struct types {};

template <typename T, typename... U>
struct types_link
{
    using type = types<T, U...>;
};

template <typename T, typename... U>
struct types_link<T, types<U...>>
{
    using type = types<T, U...>;
};

template <typename... T, typename U>
struct types_link<types<T...>, U>
{
    using type = types<T..., U>;
};

template<int RestI, typename ArgsTuple, typename... P>
struct UnboundArgType
{
    static constexpr std::size_t size = std::tuple_size<ArgsTuple>::value;
    
    using type_this = typename std::tuple_element<size - RestI, ArgsTuple>::type;
    using type_list = typename types_link<P...,type_this>::type;
    
    using type = typename UnboundArgType<RestI - 1, ArgsTuple, type_list>::type;
};


template<typename ArgsTuple, typename... P>
struct UnboundArgType<0,ArgsTuple, P...>
{
    
    using type = typename std::conditional<(sizeof...(P) == 0), types<>, types<P...>>::type ;
};

template<typename ArgsTuple, typename... P>
struct UnboundArgType<0,ArgsTuple, types<P...>> : public UnboundArgType<0,ArgsTuple, P...>{};


template<int N, typename T, typename U, typename... P>
struct unbound_type
{
private:
    
    using t_type = typename std::tuple_element<N-1,T>::type;
    using u_type = typename std::tuple_element<N-1,U>::type;
    
    using maybe_type = typename types_link<t_type,P...>::type;
    
    using type_this = typename std::conditional<(std::is_placeholder<u_type>::value > 0), maybe_type, types<P...>>::type ;

public:
    using type = typename unbound_type<N-1, T, U, type_this>::type;
    
};


template<int N, typename T, typename U, typename... P>
struct unbound_type<N,T,U,types<P...>> : unbound_type<N, T, U, P...>{};

template<typename T, typename U, typename... P>
struct unbound_type<0, T, U, P...>
{
    using type = types<P...>;
};


template<typename T, typename U, typename... P>
struct unbound_type<0, T, U, types<P...>> : unbound_type<0, T, U, P...> {};


template <bool IsWeakCall, typename F,typename Runner, typename... UnboundArgs>
struct make_callback_ex;

template <bool IsWeakCall, typename F, typename Runner,typename... UnboundArgs>
struct make_callback_ex<IsWeakCall, F, Runner, types<UnboundArgs...>>
{
    
    using return_type = typename function_traits<F>::return_type;

    using type = Callback<return_type(UnboundArgs...)>;
    
    using storage_type = BindStorage<IsWeakCall, return_type, Runner, UnboundArgs... >;
};



template <typename R, typename... UnboundArgs>
struct make_callback;


template <typename R, typename... UnboundArgs>
struct make_callback<R, types<UnboundArgs...>>
{
    using type = Callback<R(UnboundArgs...)>;
    
    using storage_type = BindStorage<false, R, void, UnboundArgs... >;
};

template <typename R>
struct make_callback<R, types<void>>
{
    using type = Callback<R(void)>;
    
    using storage_type = BindStorage<false, R, void >;
};


template<typename First, typename... Rest>
struct UnboundArgsNum
{
    enum { value = UnboundArgsNum<First>::value + UnboundArgsNum<Rest...>::value };
};

template<typename Last>
struct UnboundArgsNum<Last>
{
    enum{ value = (std::is_placeholder<Last>::value > 0 ? 1 : 0) };
};

template<typename F, typename... Args>
decltype(auto) Bind(F&& function, const Args&... args)
{
    using FuncArgsTuple = typename function_traits<F>::tuple_type;
    
    using BoundArgsNumType = BoundArgsNum<std::is_member_function_pointer<F>::value, Args...> ;
    
    // if method set 1
    static constexpr std::size_t MethodRunnerArg = std::is_member_function_pointer<F>::value ? 1 : 0;
    
    using Unbound = typename UnboundArgType<function_traits<F>::arity - BoundArgsNumType::size - MethodRunnerArg,FuncArgsTuple>::type;
    
    using IsWeakCall = IsWeakMethodEx<std::is_member_function_pointer<F>::value, Args...>;
    using MethodRunner = typename RunnerType<std::is_member_function_pointer<F>::value, Args...>::type;
    
    using callback_type = typename make_callback_ex<IsWeakCall::value, F, MethodRunner,Unbound>::type;
    using st_type = typename make_callback_ex<IsWeakCall::value, F,MethodRunner,Unbound>::storage_type;
    
    return callback_type(new st_type(function, args...));
}

typedef Callback<void(void)> Closure;


#endif /* __BIND_H__ */
