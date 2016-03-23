/* -------------------------------------------------------------------------
//     filename :	bind_storage.h
//     author   :	cjzhou@tencent.cn
//     created  :	2016-3-10 20:16
//     purpose  :
//
// -----------------------------------------------------------------------*/
#ifndef __BIND_STORAGE_H__
#define __BIND_STORAGE_H__

#include "callback/bind_helpers.h"
#include <functional>

class BindStorageBase
{
public:
	virtual ~BindStorageBase() {}
};

template <bool IsMemberMethod, typename RunnerType, typename... Args >
struct BindStorage;

template <typename RunnerType, typename... Args>
struct BindStorage<true, RunnerType, Args...> : public BindStorageBase
{
	typedef std::function<void()> RunnableFunc;
	typedef InvokerHelper<true, BindStorage> InvokeType;

	template<typename T, typename C, typename... Args>
	BindStorage(T&& method, C&& weakRunner, Args&&... args)
	{
		_runfunc = std::bind(method, weakRunner.lock(), args...);
		_runner = weakRunner;
	}

	RunnableFunc _runfunc;
	RunnerType _runner;
};

template <typename RunnerType, typename... Args>
struct BindStorage<false, RunnerType, Args...> : public BindStorageBase
{
	typedef std::function<void()> RunnableFunc;
	typedef InvokerHelper<false, BindStorage> InvokeType;

	template<typename T, typename C, typename... Args>
	BindStorage(T&& method, C&& runner, Args&&... args)
	{
		_runfunc = std::bind(method, runner, args...);
	}

	// for lamba or function with no params
	template<typename T>
	BindStorage(T&& function)
	{
		_runfunc = std::bind(function);
	}

	RunnableFunc _runfunc;
};


#endif /* __BIND_STORAGE_H__ */