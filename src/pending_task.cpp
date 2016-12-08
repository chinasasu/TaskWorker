#include "pending_task.h"

PendingTask::PendingTask(const TrackedLocation& postedFrom, const Closure& task)
	:posted_time_(std::chrono::steady_clock::now()),
	posted_from(postedFrom),
	task(task),
	sequence(0),
	nestable(true)
{

}

PendingTask::PendingTask(const TrackedLocation& postedFrom, const Closure& task, TimerTicks delayedTimeToRun, bool bNestable)
	:posted_time_(std::chrono::steady_clock::now()),
	delayedTimeToRun(delayedTimeToRun),
	posted_from(postedFrom),
	task(task),
	sequence(0),
	nestable(bNestable)
{

}

PendingTask::PendingTask():
	sequence(-1), nestable(false)
{

}

PendingTask::~PendingTask()
{

}

bool PendingTask::operator<(const PendingTask& other) const
{
	if (delayedTimeToRun < other.delayedTimeToRun)
		return false;

	if (delayedTimeToRun > other.delayedTimeToRun)
		return true;

	return (sequence - other.sequence) > 0;
}

bool PendingTask::IsValid() const
{
	return !task.IsNull();
}

bool PendingTask::IsDelayTask() const
{
	return delayedTimeToRun.time_since_epoch() != TimerTicks::duration::zero();
}

void TaskQueue::Swap(TaskQueue* queue)
{
	c.swap(queue->c);
}
