#include "message_queue.h"

MessageQueue::MessageQueue(bool quit_when_idle)
	:next_seq_num_(0),
	quit_when_idle_(quit_when_idle),
	quitting_(false)
{

}

MessageQueue::~MessageQueue()
{

}

PendingTask MessageQueue::Next()
{
	TimerTicks::duration nextWakeupTimeoutMillis(TimerTicks::duration::zero());
	for (;;)
	{
		DoPriorityWork();

		if (quitting_)
			break;

		// 1. 先处理即时任务
		// 2. 即时任务全部完成后，再处理delay事件
		// 3. 根据delay时间值，阻塞delay时间
		if (!work_task_queue_.empty())
		{
			PendingTask pendingTask = work_task_queue_.front();
			work_task_queue_.pop();

			// 如果是delay事件，添加到delay队列
 			if (pendingTask.IsDelayTask())
 			{
 				delayed_task_queue_.push(pendingTask);
 				continue;
 			}

			return pendingTask;
		}

		// _workTaskQueue 处理完成后，再处理延迟任务
		if (!delayed_task_queue_.empty())
		{
			// 这里即时任务已全部完成，检查delay时间是否到期
			PendingTask delayPendingTask = delayed_task_queue_.top();
			nextWakeupTimeoutMillis = delayPendingTask.delayedTimeToRun - TimerClock::now();
			if (nextWakeupTimeoutMillis <= TimerTicks::duration::zero())
			{
				// 时间到，返回task，进入执行状态
				delayed_task_queue_.pop();
				return delayPendingTask;
			}
		}

		DoIdleWork();

		if (quitting_)
			break;

		// 没有延时任务执行，空闲时退出
		if (nextWakeupTimeoutMillis <= TimerTicks::duration::zero()
			&& quit_when_idle_)
			break;

		// 这里了_workTaskQueue必定为空
		TryWaitTask(nextWakeupTimeoutMillis);

		// 唤醒后，立即加载一次任务到工作队列
		SwapTask();

	}

	return PendingTask();
}

bool MessageQueue::EnqueueTask(const TrackedLocation& from, const Closure& task)
{
	return EnqueueTask(from, task, TimeDelta(0));
}

bool MessageQueue::EnqueueTask(const TrackedLocation& from, const Closure& task, const TimeDelta& delay)
{
	std::lock_guard<std::mutex> queueLock(queue_mutex_);

	PendingTask pendingTask(from, task, CalcDelyatedRunTime(delay), false);
	pendingTask.sequence = next_seq_num_++;

	incoming_task_queue_.push(pendingTask);

	ScheduleWork();
	return true;
}

void MessageQueue::Quit()
{
	quitting_ = true;
}

void MessageQueue::QuitWhenIdle()
{
	quit_when_idle_ = true;
}

void MessageQueue::TryWaitTask(const TimerTicks::duration& dura_time)
{
	// 等待新的incoming task到达
	std::unique_lock<std::mutex> lk(queue_mutex_);
	if (dura_time > TimerTicks::duration::zero())
	{
		auto absTimePoint = TimerClock::now() + dura_time;
		cv_event_.wait_until(lk, absTimePoint, [&] {  return incoming_task_queue_.size() > 0; });
	}
	else
	{
		cv_event_.wait(lk, [&] {  return incoming_task_queue_.size() > 0; });
	}
}

void MessageQueue::ScheduleWork()
{
	cv_event_.notify_all();
}

bool MessageQueue::SwapTask()
{
	std::unique_lock<std::mutex> lk(queue_mutex_);

	if (work_task_queue_.empty())
		work_task_queue_.Swap(&incoming_task_queue_);	// 这里是常量时间，指针赋值操作

	return work_task_queue_.size() > 0;
}

TimerTicks MessageQueue::CalcDelyatedRunTime(const TimeDelta& delay)
{
	return std::chrono::steady_clock::now() + delay;
}