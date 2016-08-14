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

//

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


template <typename R, typename... UnboundArgs>
struct make_callback;


template <typename R, typename... UnboundArgs>
struct make_callback<R, types<UnboundArgs...>>
{
    using type = Callback<R(UnboundArgs...)>;
    
    using storage_type = BindStorage<false, R, void, UnboundArgs... >;
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

template<int N, typename Arg0, typename... Args>
struct bound_tuple
{
    using type = typename std::conditional<(N == (sizeof...(Args) + 1)),
    typename std::tuple<Arg0, Args...>,
    typename std::tuple<Args...> >::type ;
};


// 完全绑定运行时不再需要参数
template<typename F, typename... Args,
typename std::enable_if<((sizeof...(Args) != 0) && (UnboundArgsNum<Args...>::value == 0)),int>::type* = nullptr >
decltype(auto) Bind(F function, Args... args)
{
    using return_type = typename function_traits<F>::return_type;
    
    using storage_type = BindStorage<false, return_type, void >;
    
    return Callback<return_type(void)>(new storage_type(function, args...));
}

// 完全没有参数绑定
template<typename F, typename... Args,
typename std::enable_if<((sizeof...(Args) == 0)),int>::type* = nullptr >
decltype(auto) Bind(F function, Args... args)
{
    using return_type = typename function_traits<F>::return_type;
    
    using storage_type = BindStorage<false, return_type, void >;
    
    return Callback<return_type(void)>(new storage_type(function, args...));
}

// 运行时需要参数
template<typename F, typename... Args,
typename std::enable_if<(UnboundArgsNum<Args...>::value > 0),int>::type* = nullptr >
decltype(auto) Bind(F&& function, Args... args)
{
    using return_type = typename function_traits<F>::return_type;
    using bound_type = typename bound_tuple<function_traits<F>::arity, Args...>::type;
    
    using unbound_type = typename unbound_type<function_traits<F>::arity,
    typename function_traits<F>::tuple_type,
    bound_type>::type;
    
    using call_back = typename make_callback<return_type,unbound_type>::type;
    
    using storage_type = typename make_callback<return_type, unbound_type>::storage_type;
    
    return call_back(new storage_type(function, args...));
}


typedef Callback<void(void)> Closure;

//typedef Callback<bool(void)> BoolCallback;



#endif /* __BIND_H__ */