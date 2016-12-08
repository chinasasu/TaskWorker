#include "task_runner.h"

#include "tracker.h"
#include "looper.h"

class PostTaskAndReplyWrapper
{
public:
	explicit PostTaskAndReplyWrapper(
		const TrackedLocation& from_here,
		Closure& task,
		Closure& reply)
		:from_here_(from_here), origin_runner_(Looper::ThisLooper()->task_runner())
	{
		task_ = task;
		reply_ = reply;
	}

	void Run()
	{
		task_.Run();

		origin_runner_->PostTask(from_here_,
			Bind(&PostTaskAndReplyWrapper::RunReplyAndSelfRelease, this));
	}

private:
	void RunReplyAndSelfRelease()
	{
		task_.Reset();
		reply_.Run();

		delete this;
	}

private:
	std::shared_ptr<TaskRunner> origin_runner_;
	TrackedLocation from_here_;
	Closure task_;
	Closure reply_;
};



TaskRunner::TaskRunner(std::shared_ptr<MessageQueue> mq)
	:mq_(mq)
{

}

TaskRunner::~TaskRunner()
{
}

bool TaskRunner::PostTask(const TrackedLocation& from_here, Closure& task)
{
	if (mq_.get())
	{
		mq_->EnqueueTask(from_here, task);
		return true;
	}
	return false;
}

bool TaskRunner::PostDelayedTask(const TrackedLocation& from_here, Closure& task, const TimeDelta& delay)
{
	if (mq_.get())
	{
		mq_->EnqueueTask(from_here, task, delay);
		return true;
	}
	return false;
}

void TaskRunner::PostTaskAndReply(const TrackedLocation& from_here, Closure& task, Closure& reply)
{
	PostTaskAndReplyWrapper* reply_wrapper = new PostTaskAndReplyWrapper(from_here, task, reply);
	if (!PostTask(from_here, Bind(&PostTaskAndReplyWrapper::Run, reply_wrapper)))
		delete reply_wrapper;
}

bool TaskRunner::RunTasksOnCurrentThread() const
{
	Looper* looper = Looper::ThisLooper();
	if (looper && looper->GetMessageQueue() == mq_.get())
		return true;
	return false;
}

