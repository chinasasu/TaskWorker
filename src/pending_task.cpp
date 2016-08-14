#include "pending_task.h"

TrackedLocation::TrackedLocation(const char* functionName,
	const char* fileName, 
	int line, 
	const void* programCounter)
	: _functionName(functionName),
	_fileName(fileName),
	_lineNumber(line),
	_programCounter(programCounter)
{

}

TrackedLocation::TrackedLocation()
	:_functionName("Unknown"),
	_fileName("Unknown"),
	_lineNumber(-1),
	_programCounter(NULL)
{

}



const void* GetProgramCounter()
{
	return _ReturnAddress();
}

PendingTask::PendingTask(const TrackedLocation& postedFrom, const Closure& task)
	:postedTime(std::chrono::steady_clock::now()),
	postedFrom(postedFrom),
	task(task),
	sequenceNum(0),
	nestable(true)
{

}

PendingTask::PendingTask(const TrackedLocation& postedFrom, const Closure& task, TimerTicks delayedTimeToRun, bool bNestable)
	:postedTime(std::chrono::steady_clock::now()),
	delayedTimeToRun(delayedTimeToRun),
	postedFrom(postedFrom),
	task(task),
	sequenceNum(0),
	nestable(bNestable)
{

}

PendingTask::PendingTask():
	sequenceNum(-1), nestable(false)
{

}

bool PendingTask::operator<(const PendingTask& other) const
{
	if (delayedTimeToRun < other.delayedTimeToRun)
		return false;

	if (delayedTimeToRun > other.delayedTimeToRun)
		return true;

	return (sequenceNum - other.sequenceNum) > 0;
}

bool PendingTask::IsValid() const
{
	return !task.IsNull();
}

bool PendingTask::IsDelayTask() const
{
	return delayedTimeToRun.time_since_epoch() != TimerTicks::duration::zero();
}

PendingTask::~PendingTask()
{

}

void TaskQueue::Swap(TaskQueue* queue)
{
	c.swap(queue->c);
}
