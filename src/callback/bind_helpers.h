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

template <typename T>
struct IsWeakMethod : public std::false_type {};

template <typename T>
struct IsWeakMethod<std::weak_ptr<T> > : public std::true_type {};


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