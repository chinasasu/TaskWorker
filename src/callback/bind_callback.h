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


template <typename R, typename... UnboundArgs>
class Callback<R(UnboundArgs...)> : public CallbackBase
{
public:

    Callback() : CallbackBase(NULL) { }
    
    template<bool IsMethod, typename Runnable, typename BoundArgsTuple, typename RunnerType>
    Callback(BindStorage<IsMethod, R, Runnable, RunnerType, BoundArgsTuple>* bindState)
    :CallbackBase(bindState)
    {
        PolymorphicInvoke invoker = &BindStorage<IsMethod, R, Runnable, RunnerType, BoundArgsTuple>::InvokeType::Run;
        
        invokefunc_ = reinterpret_cast<InvokeFuncStorage>(invoker);
    }
    
    R Run(UnboundArgs... args)
    {
        PolymorphicInvoke f = reinterpret_cast<PolymorphicInvoke>(invokefunc_);
        return f(bindStorage_.get(), args...);
    }
    
    bool Equals(const Callback& other) const
    {
        return CallbackBase::Equals(other);
    }
    
private:
    typedef R(*PolymorphicInvoke)(BindStorageBase*, UnboundArgs...);
};

#endif /* __BIND_CALLBACK_H__ */