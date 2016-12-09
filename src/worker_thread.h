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
	// ȷ���Ƿ��ѳɹ�������־
	bool _started;

	// ����ֹͣ��
	bool _stopping;

	// ��������
	bool _running;


	struct StartupData;
	StartupData* startup_data_;

	std::thread thread_;
	MessageQueue* mq_;
	std::shared_ptr<TaskRunner> runner_;
};


#endif /* __WORKER_THREAD_H__ */