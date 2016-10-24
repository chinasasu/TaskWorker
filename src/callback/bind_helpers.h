/* -------------------------------------------------------------------------
//     filename :	bind_helpers.h
//     author   :	cjzhou@tencent.cn
//     created  :	2016-3-10 20:19
//     purpose  :
//
// -----------------------------------------------------------------------*/
#ifndef __BIND_HELPERS_H__
#define __BIND_HELPERS_H__

#include <memory>
#include <functional>


class BindStorageBase;

template <bool IsMethod, typename... Args>
struct IsWeakMethod : public std::false_type {};

template <typename T, typename... Args>
struct IsWeakMethod<true, std::weak_ptr<T>, Args...> : public std::true_type {};

template <typename T, typename... Args>
struct IsWeakMethod<true, std::weak_ptr<T>&, Args...> : public std::true_type {};

template <typename T, typename... Args>
struct IsWeakMethod<true, const std::weak_ptr<T>&, Args...> : public std::true_type {};

template <bool IsMethod, typename... Args>
struct MethodRunnerTraits
{
	using type = void;
};

template <typename T, typename... Args>
struct MethodRunnerTraits<true, T, Args...>
{
	using type = T;
};


template <bool IsMethod, typename... Args>
struct BoundArgsSize;

template<typename T, typename... Args>
struct BoundArgsSize<true, T, Args...>
{
	static constexpr std::size_t size = sizeof...(Args);
};

template<typename... Args>
struct BoundArgsSize<false, Args...>
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
		using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
	};

	using tuple_type = std::tuple<Args...>;
};

// member function pointer
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...)> : public function_traits<R(C&, Args...)>
{};

// weakptr member function pointer
template<class C, class R, class T, class... Args>
struct function_traits<R(C::*)(std::weak_ptr<T>, Args...)> : public function_traits<R(C&, T*, Args...)>
{};

// const member function pointer
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...) const> : public function_traits<R(C&, Args...)>
{};

// member object pointer
template<class C, class R>
struct function_traits<R(C::*)> : public function_traits<R(C&)>
{};


template <typename...>
struct TypeList {};

template <typename T, typename... U>
struct TypesLink
{
	using type = TypeList<T, U...>;
};

template <typename T, typename... U>
struct TypesLink<T, TypeList<U...>>
{
	using type = TypeList<T, U...>;
};

template <typename... T, typename U>
struct TypesLink<TypeList<T...>, U>
{
	using type = TypeList<T..., U>;
};

template<int RestI, typename ArgsTuple, typename... P>
struct UnboundTypeTraits
{
	static constexpr std::size_t ArgSize = std::tuple_size<ArgsTuple>::value;

	using type_this = typename std::tuple_element<ArgSize - RestI, ArgsTuple>::type;
	using type_list = typename TypesLink<P..., type_this>::type;

	using type = typename UnboundTypeTraits<RestI - 1, ArgsTuple, type_list>::type;
};

template<typename ArgsTuple, typename... P>
struct UnboundTypeTraits<0, ArgsTuple, P...>
{
	using type = typename std::conditional<(sizeof...(P) == 0), TypeList<>, TypeList<P...>>::type;
};

template<typename ArgsTuple, typename... P>
struct UnboundTypeTraits<0, ArgsTuple, TypeList<P...>> : public UnboundTypeTraits<0, ArgsTuple, P...> {};

template <bool IsWeakCall, typename Storage, typename R, typename... Args>
struct InvokerHelper;

template <typename StorageType, typename R, typename... Args>
struct InvokerHelper<false, StorageType, R, Args...>
{
    template<typename... UnboundArgs>
    static R Run(BindStorageBase* base, UnboundArgs... args)
    {
        StorageType* storage = static_cast<StorageType*>(base);
        
        return storage->_runfunc(args...);
    }
};

template <typename StorageType, typename R, typename... Args>
struct InvokerHelper<true, StorageType, R, Args...>
{
    template<typename... UnboundArgs>
    static R Run(BindStorageBase* base, UnboundArgs... args)
    {
        StorageType* storage = static_cast<StorageType*>(base);
        
        if (storage->_runner.expired())
            return static_cast<R>(0);
        
        return storage->_runfunc(args...);
    }
};


#endif /* __BIND_HELPERS_H__ */
