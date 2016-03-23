/* -------------------------------------------------------------------------
//     filename :	bind_callback.h
//     author   :	cjzhou@tencent.cn
//     created  :	2016-3-10 20:21
//     purpose  :
//
// -----------------------------------------------------------------------*/
#ifndef __BIND_CALLBACK_H__
#define __BIND_CALLBACK_H__

#include "callback/bind_storage.h"

class BindStorageBase;

class CallbackBase
{
public:
	bool IsNull() const;
	void Reset();

protected:

	explicit CallbackBase(BindStorageBase* bindStorage);
	~CallbackBase();

	bool Equals(const CallbackBase& other) const;

	typedef void(*InvokeFuncStorage)(void);

	std::shared_ptr<BindStorageBase> bindStorage_;
	InvokeFuncStorage invokefunc_;
};


template <typename Sig>
class Callback;

template <typename R>
class Callback<R(void)> : public CallbackBase
{
public:
	Callback() : CallbackBase(NULL) { }

	template<bool IsWeak, typename RunnerType, typename... Args>
	Callback(BindStorage<IsWeak, RunnerType, Args...>* bindState)
		:CallbackBase(bindState)
	{
		PolymorphicInvoke invoker = &BindStorage<IsWeak, RunnerType, Args...>::InvokeType::Run;

		invokefunc_ = reinterpret_cast<InvokeFuncStorage>(invoker);
	}

	R Run()
	{
		PolymorphicInvoke f = reinterpret_cast<PolymorphicInvoke>(invokefunc_);
		return f(bindStorage_.get());
	}

	bool Equals(const Callback& other) const 
	{
		return CallbackBase::Equals(other);
	}

private:
	typedef R(*PolymorphicInvoke)(BindStorageBase*);
};

#endif /* __BIND_CALLBACK_H__ */