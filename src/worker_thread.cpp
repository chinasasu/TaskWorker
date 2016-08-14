#include "worker_thread.h"
#include "message_queue.h"
#include "looper.h"


WorkerThread::Options::Options()
{

}

WorkerThread::Options::~Options()
{

}


WorkerThread::WorkerThread(const char* name)
	:_name(name)
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
	if(_started)

	_thread.join();

	return true;
}

void WorkerThread::StopSoon()
{
	//_mq->EnqueueTask(FROM_HERE, Bind(&) )
}

bool WorkerThread::StartWithOptions(const Options& options)
{
	_thread = std::thread(&WorkerThread::ThreadRun, this);
	
	_started = true;
	return true;
}

void WorkerThread::ThreadRun()
{
	Looper::Prepare();

	//_mq = Looper::ThisLooper()->Loop();

	Looper::Loop();
}

