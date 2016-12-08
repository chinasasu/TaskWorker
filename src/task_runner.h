// Copyright (c) 2016 Tencent Inc. All rights reserved.
// Author : cjzhou@tencent.com
// Created: 2016-10-10 8:55

#ifndef __TASK_RUNNER_H__
#define __TASK_RUNNER_H__

#include <memory>

#include "pending_task.h"
#include "callback/bind.h"
#include "message_queue/message_queue.h"

class TaskRunner
{
public:
	explicit TaskRunner(std::shared_ptr<MessageQueue> mq);
	~TaskRunner();

	bool PostTask(const TrackedLocation& from_here, Closure& task);
	bool PostDelayedTask(const TrackedLocation& from_here, Closure& task, const TimeDelta& delay);
	void PostTaskAndReply(const TrackedLocation& from_here, Closure& task, Closure& reply);

	bool RunTasksOnCurrentThread() const;

private:
	std::shared_ptr<MessageQueue> mq_;
};

#endif /* __TASK_RUNNER_H__ */