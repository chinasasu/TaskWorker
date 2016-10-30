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


class BindStorageBase
{
public:
	virtual ~BindStorageBase() {}
};

template <bool IsMemberMethod, typename ReturnType, typename RunnerType, class... Args >
struct BindStorage;

template <typename ReturnType, typename RunnerType, typename... UnboundArgs>
struct BindStorage<true, ReturnType, RunnerType, UnboundArgs...> : public BindStorageBase
{
	typedef std::function<ReturnType(UnboundArgs...)> RunnableFunc;
	typedef InvokerHelper<true, BindStorage, ReturnType, UnboundArgs...> InvokeType;

	template<typename T, typename C, typename... Args>
	BindStorage(T&& method, C&& weakRunner, Args&&... args)
	{
        MakeStore(make_int_parameter_pack<sizeof...(UnboundArgs)>(), method, weakRunner.lock(), args...);
		_runner = weakRunner;
	}

	RunnableFunc _runfunc;
	RunnerType _runner;
    
private:
    
    template<int... Is, typename T, typename... Args>
    void MakeStore(int_parameter_pack<Is...>, T&& method, Args&&... args)
    {
        _runfunc = std::bind(method, args..., std::custom_placeholder<Is>()...);
        
    }

};

template <typename ReturnType, typename RunnerType, typename... UnboundArgs>
struct BindStorage<false, ReturnType, RunnerType, UnboundArgs...> : public BindStorageBase
{
	typedef std::function<ReturnType(UnboundArgs...)> RunnableFunc;
	typedef InvokerHelper<false, BindStorage, ReturnType, UnboundArgs...> InvokeType;
    
	template<typename T, typename... Args>
	BindStorage(T&& method, Args&&... args)
	{
        MakeStore(make_int_parameter_pack<sizeof...(UnboundArgs)>(), method, args...);
	}

//	// for lamba or function with no params
//	template<typename T>
//	BindStorage(T&& function)
//	{
//		_runfunc = std::bind(function);
//	}
    
    RunnableFunc _runfunc;
    
private:
    
    template<int... Is, typename T, typename... Args>
    void MakeStore(int_parameter_pack<Is...>, T&& method, Args&&... args)
    {
        _runfunc = std::bind(method, args..., std::custom_placeholder<Is>()...);

    }

};





#endif /* __BIND_STORAGE_H__ */
