/* -------------------------------------------------------------------------
//     filename :	bind_storage.h
//     author   :	cjzhou@tencent.cn
//     created  :	2016-3-10 20:16
//     purpose  :
//
// -----------------------------------------------------------------------*/
#ifndef __BIND_STORAGE_H__
#define __BIND_STORAGE_H__

#include <functional>

#include "callback/bind_helpers.h"


// http://stackoverflow.com/questions/26129933/bind-to-function-with-an-unknown-number-of-arguments-in-c
namespace std{
    
    template<int>
    struct custom_placeholder
    {
        
    };
    
    template<int N>
    struct is_placeholder<custom_placeholder<N>> : std::integral_constant<int, N>{};
}

template <int...>
struct int_parameter_pack
{
    
};

template<int N, int... Is>
struct make_int_parameter_pack : make_int_parameter_pack<N-1, N, Is...>
{
    
};

template<int... Is>
struct make_int_parameter_pack<0, Is...> : int_parameter_pack<Is...>
{
    
};

template <typename T>
struct IsWeakRunner : public std::false_type {};

template <typename T>
struct IsWeakRunner<std::weak_ptr<T>> : public std::true_type {};


class BindStorageBase
{
public:
	virtual ~BindStorageBase() {}
};

template <bool IsMemberMethod, typename ReturnType, typename Runnable, typename RunnerType, typename BoundArgsTuple >
struct BindStorage;

template <typename ReturnType, typename Runnable, typename RunnerType, typename BoundArgsTuple>
struct BindStorage<true, ReturnType, Runnable, RunnerType, BoundArgsTuple> : public BindStorageBase
{
	typedef InvokeHelper<true, BindStorage, ReturnType> InvokeType;
	typedef RunnerType RunnerType;
	
	template<typename Runnable, typename RunnerType, typename... Args>
	BindStorage(Runnable runnable, RunnerType&& runner, Args&&... args)
		:runnable_(runnable), runner_(runner)
	{
		bound_args_ = std::make_tuple(args...);
	}

	Runnable runnable_;
	RunnerType runner_;

	BoundArgsTuple bound_args_;
};

template <typename ReturnType, typename Runnable, typename RunnerType, typename BoundArgsTuple>
struct BindStorage<false, ReturnType, Runnable, RunnerType, BoundArgsTuple> : public BindStorageBase
{
	typedef InvokeHelper<false, BindStorage, ReturnType> InvokeType;
	typedef RunnerType RunnerType;

	template<typename Runnable, typename... Args>
	BindStorage(Runnable&& runnable, Args&&... args)
		:runnable_(runnable)
	{
		bound_args_ = std::make_tuple(args...);
	}

	BoundArgsTuple bound_args_;
	Runnable runnable_;
};





#endif /* __BIND_STORAGE_H__ */
