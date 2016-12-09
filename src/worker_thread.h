/* -------------------------------------------------------------------------
//     filename :	worker_thread.h
//     author   :	cjzhou@tencent.cn
//     created  :	2016-4-5 20:09
//     purpose  :
//
// -----------------------------------------------------------------------*/
#ifndef __WORKER_THREAD_H__
#define __WORKER_THREAD_H__

#include <string>
#include <thread>

#include "looper.h"
#include "task_runner.h"

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
	WorkerThread(const char* name, Looper* looper);

	virtual ~WorkerThread();

	bool Start();
	bool Stop();
	void StopSoon();
	bool StartWithOptions(const Options& options);

	MessageQueue* message_queue() const { return mq_; }
	std::shared_ptr<TaskRunner> task_runner() const { return runner_; }
	bool RunTasksOnCurrentThread() const;

private:
	void ThreadRun();

protected:
private:
	const std::string thread_name_;
	// 确定是否已成功启动标志
	bool _started;

	// 正在停止中
	bool _stopping;

	// 正在运行
	bool _running;


	struct StartupData;
	StartupData* startup_data_;

	std::thread thread_;
	MessageQueue* mq_;
	std::shared_ptr<TaskRunner> runner_;
};


#endif /* __WORKER_THREAD_H__ */