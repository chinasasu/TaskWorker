// Copyright (c) 2016 Tencent Inc. All rights reserved.
// Author : cjzhou@tencent.com
// Created: 2016-11-18 9:41

#ifndef __WAITABLE_EVENT_H__
#define __WAITABLE_EVENT_H__

#include <mutex>

/*
eg.

	WaitableEvent wait_event;
	wait_event.Wait();

*/
class WaitableEvent
{
public:
	WaitableEvent();
	~WaitableEvent();

	/*
		使信号激发，wait的线程会立即可以继续
	*/
	void Signal();

	/*
		等待事件触发后继续
	*/
	void Wait();

private:
	std::mutex mutex_;
	std::condition_variable cv_;
	bool signaled_;
};

#endif /* __WAITABLE_EVENT_H__ */