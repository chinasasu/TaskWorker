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

class TrackedLocation
{
public:
	TrackedLocation(const char* functionName,
		const char* fileName,
		int line,
		const void* programCounter);

	TrackedLocation();

protected:
private:
	const char* _functionName;
	const char* _fileName;
	int _lineNumber;
	const void* _programCounter;
};

const void* GetProgramCounter();

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
	TrackedLocation postedFrom;
	
	int sequenceNum;
	bool nestable;

	TimerTicks postedTime;
	TimerTicks delayedTimeToRun;
};


class TaskQueue : public std::queue<PendingTask>
{
public:
	void Swap(TaskQueue* queue);
};

typedef std::priority_queue<PendingTask> DelayedTaskQueue;


// Define a macro to record the current source location.
#define FROM_HERE FROM_HERE_WITH_EXPLICIT_FUNCTION(__FUNCTION__)

#define FROM_HERE_WITH_EXPLICIT_FUNCTION(function_name)                        \
    TrackedLocation(function_name,                                 \
                                __FILE__,                                      \
                                __LINE__,                                      \
                                GetProgramCounter())

#endif /* __PENDING_TASK_H__ */
