// Copyright (c) 2016 Tencent Inc. All rights reserved.
// Author : cjzhou@tencent.com
// Created: 2016-10-12 15:08

#ifndef __WEAKPTR_FACTORY_H__
#define __WEAKPTR_FACTORY_H__

#include <memory>

/*
Usage:
	class Foo
	{
	public:
		Foo() : weak_factory_(this)	{ }

		std::weak_ptr<Foo> GetWeakPtr()
		{
			return weak_factory_.GetWeakPtr();
		}

	private:
		WeakPtrFactory<Foo> weak_factory_;
	};
*/
template<class T>
class WeakPtrFactory
{
public:
	explicit WeakPtrFactory(T* ptr)
		:ptr_(ptr)
	{
	}
	~WeakPtrFactory()
	{
		ptr_ = NULL;
	}

	std::weak_ptr<T> GetWeakPtr()
	{
		if (!owner_.get())
			owner_ = std::shared_ptr<T>(ptr_, [](T* p) {});

		return std::weak_ptr<T>(owner_);
	}

private:
	std::shared_ptr<T> owner_;
	T* ptr_;
};

#endif /* __WEAKPTR_FACTORY_H__ */