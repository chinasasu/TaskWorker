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

/*
 1. 分离绑定参数和运行时参数
 2. 生成std::function变量存储
 3. Run时绑定运行时参数
 */

// https://functionalcpp.wordpress.com/2013/08/05/function-traits/
// http://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda
template<class F>
struct function_traits;

// lamba
template <typename T>
struct function_traits
	: public function_traits<decltype(&T::operator())>
{};


// const member function pointer
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>
	: public function_traits<ReturnType(Args...)>
{
	typedef const ClassType& owner_type;
};

// member function pointer
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...)>
	: public function_traits<ReturnType(Args...)>
{
	typedef ClassType& owner_type;
};

// function pointer
template<class R, class... Args>
struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
{};
// For generic types, directly use the result of the signature of its 'operator()'

template <typename ReturnType, typename... Args>
struct function_traits<ReturnType(Args...)>
{
	/**
	.. type:: type result_type
	The type returned by calling an instance of the function object type *F*.
	*/
	typedef ReturnType return_type;

	/**
	.. type:: type function_type
	The function type (``R(T...)``).
	*/
	typedef ReturnType function_type(Args...);

	/**
	.. data:: static const size_t arity
	Number of arguments the function object will take.
	*/
	enum { arity = sizeof...(Args) };

	/**
	.. type:: type arg<n>::type
	The type of the *n*-th argument.
	*/
	template <size_t i>
	struct argument
	{
		typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
	};

	using tuple_type = std::tuple<Args...>;
};

// 提取成员方法的类类型
template <bool IsMethod, typename... Args>
struct RunnerTraits
{
    static constexpr std::size_t BoundSize = sizeof...(Args);
        
	using RunnerType = void;
	using RunnableTuple = std::tuple<Args...>;
};

template <typename T, typename... Args>
struct RunnerTraits<true, T, Args...>
{
    static constexpr std::size_t BoundSize = sizeof...(Args);
    
	using RunnerType = T;
	using RunnableTuple = std::tuple<Args...>;
};



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
struct TypeListTraits
{
	static constexpr std::size_t ArgSize = std::tuple_size<ArgsTuple>::value;

	using type_this = typename std::tuple_element<ArgSize - RestI, ArgsTuple>::type;
	using type_list = typename TypesLink<P..., type_this>::type;

	using type = typename TypeListTraits<RestI - 1, ArgsTuple, type_list>::type;
};

template<typename ArgsTuple, typename... P>
struct TypeListTraits<0, ArgsTuple, P...>
{
	using type = typename std::conditional<(sizeof...(P) == 0), TypeList<>, TypeList<P...>>::type;
};

template<typename ArgsTuple, typename... P>
struct TypeListTraits<0, ArgsTuple, TypeList<P...>> : public TypeListTraits<0, ArgsTuple, P...> {};


class BindStorageBase;

// pure ptr
template <bool IsMethodCall, typename ReturnType, typename Runner>
struct InvokeWrapper
{
	template<typename Runnable, typename BoundTuple, std::size_t... Is, typename... Args>
	static ReturnType Invoke(Runner runner, Runnable runable, BoundTuple boundArgs, std::index_sequence<Is...>, Args... args)
	{
		// https://blogs.msdn.microsoft.com/the1/2004/08/06/using-c-member-function-pointers/
		return (runner->*runable)(std::get<Is>(boundArgs)..., args...);
	}
};

// sharedptr
template <typename ReturnType, typename Runner>
struct InvokeWrapper<true, ReturnType, std::shared_ptr<Runner>>
{
	template<typename Runnable, typename BoundTuple, std::size_t... Is, typename... Args>
	static ReturnType Invoke(std::shared_ptr<Runner> runner, Runnable runable, BoundTuple boundArgs, std::index_sequence<Is...>, Args... args)
	{
		return (runner.get()->*runable)(std::get<Is>(boundArgs)..., args...);
	}
};

// weakptr
template <typename ReturnType, typename Runner>
struct InvokeWrapper<true, ReturnType, std::weak_ptr<Runner>>
{
	template<typename Runnable, typename BoundTuple, std::size_t... Is, typename... Args>
	static ReturnType Invoke(std::weak_ptr<Runner> runner, Runnable runable, BoundTuple boundArgs, std::index_sequence<Is...>, Args... args)
	{
		if (runner.expired())
			return static_cast<ReturnType>(0);

		return (runner.lock().get()->*runable)(std::get<Is>(boundArgs)..., args...);
	}
};

// global function
template <typename ReturnType, typename Runner>
struct InvokeWrapper<false, ReturnType, Runner>
{
	template<typename Runnable, typename BoundTuple, std::size_t... Is, typename... Args>
	static ReturnType Invoke(Runnable runable, BoundTuple boundArgs, std::index_sequence<Is...>, Args... args)
	{
		return runable(std::get<Is>(boundArgs)..., args...);
	}
};


template <bool IsMethodCall, typename Storage, typename ReturnType, typename RunnerType>
struct InvokeHelper;

// 成员函数
template <typename StorageType, typename ReturnType, typename RunnerType>
struct InvokeHelper<true, StorageType, ReturnType, RunnerType>
{
    template<typename... UnboundArgs>
    static ReturnType Run(BindStorageBase* base, UnboundArgs... args)
    {
        StorageType* storage = static_cast<StorageType*>(base);
		typedef InvokeWrapper<true, ReturnType, RunnerType> InvokeType;

		constexpr auto tuple_size = std::tuple_size<decltype(storage->bound_args_)>::value;
		return InvokeType::Invoke(storage->runner_, 
								storage->runnable_,
								storage->bound_args_,
								std::make_index_sequence<tuple_size>(),
								args...);
	}
};

// 静态或全局函数
template <typename StorageType, typename ReturnType, typename RunnerType>
struct InvokeHelper<false, StorageType, ReturnType, RunnerType>
{
	template<typename... UnboundArgs>
	static ReturnType Run(BindStorageBase* base, UnboundArgs... args)
	{
		StorageType* storage = static_cast<StorageType*>(base);
		typedef InvokeWrapper<false, ReturnType, RunnerType> InvokeType;

		constexpr auto tuple_size = std::tuple_size<decltype(storage->bound_args_)>::value;
		return InvokeType::Invoke(storage->runnable_,
			storage->bound_args_,
			std::make_index_sequence<tuple_size>(),
			args...);
	}
};

#endif /* __BIND_HELPERS_H__ */
