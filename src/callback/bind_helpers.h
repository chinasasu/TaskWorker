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

class BindStorageBase;

template <typename T>
struct IsWeakMethod : public std::false_type {};

template <typename T>
struct IsWeakMethod<std::weak_ptr<T>> : public std::true_type {};


template <bool IsWeakCall, typename Storage>
struct InvokerHelper;

template <typename StorageType>
struct InvokerHelper<false, StorageType>
{
	static void Run(BindStorageBase* base)
	{
		StorageType* storage = static_cast<StorageType*>(base);

		storage->_runfunc();
	}
};

template <typename StorageType>
struct InvokerHelper<true, StorageType>
{
	static void Run(BindStorageBase* base)
	{
		StorageType* storage = static_cast<StorageType*>(base);

		if (storage->_runner.expired())
			return;

		storage->_runfunc();
	}
};

#endif /* __BIND_HELPERS_H__ */