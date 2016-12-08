/* -------------------------------------------------------------------------
//     filename :	pending_task.h
//     author   :	cjzhou@tencent.cn
//     created  :	2016-3-28 20:54
//     purpose  :
//
// -----------------------------------------------------------------------*/
#ifndef __PENDING_TASK_H__
#define __PENDING_TASK_H__

#include <chrono>
#include <queue>

#include "callback/bind.h"
#include "tracker.h"

struct PendingTask
{
	typedef std::chrono::steady_clock::time_point TimerTicks;

	PendingTask();

	PendingTask(const TrackedLocation& postedFrom,
		const Closure& task);

	PendingTask(const TrackedLocation& postedFrom,
		const Closure& task,
		TimerTicks delayedTimeToRun,
		bool bNestable);

	~PendingTask();

	bool operator<(const PendingTask& other) const;
	bool IsValid() const;

	bool IsDelayTask() const;

	Closure task;
	TimerTicks delayedTimeToRun;
	
	int sequence;
	bool nestable;

	// for debug
	TrackedLocation posted_from;
	TimerTicks posted_time_;
};

class TaskQueue : public std::queue<PendingTask>
{
public:
	void Swap(TaskQueue* queue);
};

typedef std::priority_queue<PendingTask> DelayedTaskQueue;

#endif /* __PENDING_TASK_H__ */
