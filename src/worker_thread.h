/* -------------------------------------------------------------------------
//     filename :	worker_thread.h
//     author   :	cjzhou@tencent.cn
//     created  :	2016-4-5 20:09
//     purpose  :
//
// -----------------------------------------------------------------------*/
#ifndef __WORKER_THREAD_H__
#define __WORKER_THREAD_H__

#include <thread>

class MessageQueue;

class WorkerThread
{
public:
	struct Options
	{
		Options();
		~Options();

		int loop_type;
	};

	explicit WorkerThread(const char* name);

	virtual ~WorkerThread();

	bool Start();
	bool Stop();
	void StopSoon();
	bool StartWithOptions(const Options& options);

	MessageQueue* GetMessageQueue() { return _mq; }

private:
	void ThreadRun();

protected:
private:
	std::string _name;
	// 确定是否已成功启动标志
	bool _started;

	// 正在停止中
	bool _stopping;

	// 正在运行
	bool _running;

	std::thread _thread;
	MessageQueue* _mq;
};


#endif /* __WORKER_THREAD_H__ */