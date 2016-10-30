/* -------------------------------------------------------------------------
//     filename :	message_queue.h
//     author   :	cjzhou@tencent.cn
//     created  :	2016-3-28 20:21
//     purpose  :
//
// -----------------------------------------------------------------------*/
#ifndef __MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_H__

#include <mutex>

#include "callback/bind.h"
#include "pending_task.h"

typedef std::chrono::steady_clock TimerClock;
typedef TimerClock::time_point TimerTicks;
typedef std::chrono::nanoseconds TimeDelta;

class MessageQueue
{
public:
	MessageQueue(bool quit_when_idle);
	~MessageQueue();

	// maybe block
	virtual PendingTask Next();

	virtual bool EnqueueTask(const TrackedLocation& from,
					 const Closure& task);

	virtual bool EnqueueTask(const TrackedLocation& from,
					 const Closure& task,
					 const TimeDelta& delay);
	void Quit();
	void QuitWhenIdle();

protected:
	// 执行高优先级任务，在Next取到下一个task之前执行
	virtual bool DoPriorityWork() { return false; }

	// 执行空闲任务，在Next取完所有当前任务（不包括delay的task）后执行
	virtual void DoIdleWork() {}

	/* 尝试等待新的任务
	 1. 如果在执行过程中又被post了新的task，TryWaitTask会立即返回
	 2. 如果dura_time大于0，那么根据dura_time指定时间唤醒
	 3. 如果dura_time等于0，则会进入无限长时间等待
	 4. 上述等待中时，如果等待事件被激活，都会被唤醒，TryWaitTask会立即返回
	*/ 
	virtual void TryWaitTask(const TimerTicks::duration& dura_time);

	virtual void ScheduleWork();

	bool SwapTask();

private:
	
	void ReloadIncomingQueueToWorkQueue();
	TimerTicks CalcDelyatedRunTime(const TimeDelta& delay);

protected:
	// 即时任务队列
	TaskQueue _incomingTaskQueue;
	// 当前工作队列
	TaskQueue _workTaskQueue;
	// 延迟工作队列
	DelayedTaskQueue _delayedTaskQueue;

	std::condition_variable _cvEvent;

	// 任务递增序号，用于投递时间相同时的排序，保证先进入先执行
	int next_seq_num_;
	std::mutex queue_mutex_;
	bool quitting_;
	bool quit_when_idle_;

private:

};

#endif /* __MESSAGE_QUEUE_H__ */
