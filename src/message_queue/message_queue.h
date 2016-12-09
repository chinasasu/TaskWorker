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
	void ResetQuit();

protected:
	// ִ�и����ȼ�������Nextȡ����һ��task֮ǰִ��
	virtual bool DoPriorityWork() { return false; }

	// ִ�п���������Nextȡ�����е�ǰ���񣨲�����delay��task����ִ��
	virtual void DoIdleWork() {}

	/* ���Եȴ��µ�����
	 1. �����ִ�й������ֱ�post���µ�task��TryWaitTask����������
	 2. ���dura_time����0����ô����dura_timeָ��ʱ�份��
	 3. ���dura_time����0�����������޳�ʱ��ȴ�
	 4. �����ȴ���ʱ������ȴ��¼���������ᱻ���ѣ�TryWaitTask����������
	*/ 
	virtual void TryWaitTask(const TimerTicks::duration& dura_time);

	virtual void ScheduleWork();

private:
	
	void ReloadIncomingQueueToWorkQueue();
	TimerTicks CalcDelyatedRunTime(const TimeDelta& delay);
	bool SwapTask();

protected:

	// ˫������ض�����ƣ����ͻ������ʣ�����������������

	// ��ʱ�������
	TaskQueue incoming_task_queue_;
	// ��ǰ��������
	TaskQueue work_task_queue_;
	// �ӳٹ�������
	DelayedTaskQueue delayed_task_queue_;

	std::condition_variable cv_event_;

	// ���������ţ�����Ͷ��ʱ����ͬʱ�����򣬱�֤�Ƚ�����ִ��
	int next_seq_num_;
	std::mutex queue_mutex_;
	bool quitting_;
	bool quit_when_idle_;
};

#endif /* __MESSAGE_QUEUE_H__ */