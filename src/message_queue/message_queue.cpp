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
	TimerTicks::duration next_wakeup_timeout_ms(TimerTicks::duration::zero());
	for (;;)
	{
		// �������񵽹�������
		if (work_task_queue_.empty())
			SwapTask();

		// TODO ���ȶ��У���ʱδ��ʵ��
		DoPriorityWork();

		if (quitting_)
			break;

		// 1. �ȴ���ʱ����
		// 2. ��ʱ����ȫ����ɺ��ٴ���delay�¼�
		// 3. ����delayʱ��ֵ������delayʱ��
		if (!work_task_queue_.empty())
		{
			PendingTask pendingTask = work_task_queue_.front();
			work_task_queue_.pop();

			// �����delay�¼�����ӵ�delay����
 			if (pendingTask.IsDelayTask())
 			{
 				delayed_task_queue_.push(pendingTask);
 				continue;
 			}

			return pendingTask;
		}

		// _workTaskQueue ������ɺ��ٴ����ӳ�����
		if (!delayed_task_queue_.empty())
		{
			// ���Ｔʱ������ȫ����ɣ����delayʱ���Ƿ���
			PendingTask delayPendingTask = delayed_task_queue_.top();
			next_wakeup_timeout_ms = delayPendingTask.delayedTimeToRun - TimerClock::now();
			if (next_wakeup_timeout_ms <= TimerTicks::duration::zero())
			{
				// ʱ�䵽������task������ִ��״̬
				delayed_task_queue_.pop();
				return delayPendingTask;
			}
		}

		DoIdleWork();

		// ������ work_task_queue_ �ض�û����
		// û����ʱ����ִ�У�����ʱ�˳�
		if (next_wakeup_timeout_ms <= TimerTicks::duration::zero()
			&& quit_when_idle_)
			break;

		// ����ȴ�
		TryWaitTask(next_wakeup_timeout_ms);
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

void MessageQueue::ResetQuit()
{
	quitting_ = false;
	quit_when_idle_ = false;
}

void MessageQueue::TryWaitTask(const TimerTicks::duration& dura_time)
{
	// �ȴ��µ�incoming task����
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

	if (work_task_queue_.empty() && !incoming_task_queue_.empty())
		work_task_queue_.Swap(&incoming_task_queue_);	// �����ǳ���ʱ�䣬ָ�븳ֵ����

	return work_task_queue_.size() > 0;
}

TimerTicks MessageQueue::CalcDelyatedRunTime(const TimeDelta& delay)
{
	return std::chrono::steady_clock::now() + delay;
}

void MessageQueue::ReloadIncomingQueueToWorkQueue()
{
	if (work_task_queue_.empty())
		work_task_queue_.Swap(&incoming_task_queue_);
}

