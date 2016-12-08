#include "worker_thread.h"
#include "win/win_utils.h"
#include "waitable_event.h"
#include "message_queue/message_queue.h"
#include "looper.h"


struct WorkerThread::StartupData
{
	explicit StartupData(const Options& opt)
		: options(opt)
	{
		
	}

	const Options options;

	WaitableEvent wait_event;
};


WorkerThread::Options::Options()
{

}

WorkerThread::Options::~Options()
{

}


WorkerThread::WorkerThread(const char* name)
	:thread_name_(name), 
	_started(false),
	_stopping(false),
	_running(false),
	startup_data_(NULL)
{

}

WorkerThread::WorkerThread(const char* name, Looper* looper)
	: thread_name_(name),
	startup_data_(NULL),
	_started(true),
	_stopping(false),
	_running(true),
	mq_(looper->GetMessageQueue()),
	runner_(looper->task_runner())
{

}

WorkerThread::~WorkerThread()
{

}

bool WorkerThread::Start()
{
	Options options;
	options.loop_type = 0;

	return StartWithOptions(options);
}

bool WorkerThread::Stop()
{
	if (_started)
	{
		runner_->PostTask(FROM_HERE,
			Bind(&MessageQueue::Quit, mq_));
	}

	if (thread_.joinable())
		thread_.join();
	return true;
}

void WorkerThread::StopSoon()
{
	//_mq->EnqueueTask(FROM_HERE, Bind(&) )
}

bool WorkerThread::StartWithOptions(const Options& options)
{
	start_options_ = options;

	StartupData startup_data(options);
	startup_data_ = &startup_data;

	thread_ = std::thread(&WorkerThread::ThreadRun, this);
	
	startup_data_->wait_event.Wait();
	startup_data_ = NULL;

	_started = true;
	return true;
}

bool WorkerThread::RunTasksOnCurrentThread() const
{
	if (runner_)
		return runner_->RunTasksOnCurrentThread();
	return false;
}

void WorkerThread::ThreadRun()
{
	SetThreadName(GetCurrentThreadId(), thread_name_.c_str());
	Looper::Prepare((Looper::LooperType)start_options_.loop_type);

	mq_ = Looper::ThisLooper()->GetMessageQueue();
	runner_ = Looper::ThisLooper()->task_runner();

	startup_data_->wait_event.Signal();

	Looper::Loop();
}

