/* -------------------------------------------------------------------------
//     filename :	looper.h
//     author   :	cjzhou@tencent.cn
//     created  :	2016-3-25 9:15
//     purpose  :
//
// -----------------------------------------------------------------------*/
#ifndef __LOOPER_H__
#define __LOOPER_H__
#include <thread>

#include "task_runner.h"
#include "callback/bind.h"
#include "message_queue/message_queue.h"

class Looper
{
public:

	enum LooperType
	{
		kLoopDefault,
		kLoopUI,
	};

	Looper(LooperType loopType = kLoopDefault);
	virtual ~Looper();

	static void Prepare(LooperType loopType = kLoopDefault);
	static void Reset();
	static void Clean();

	// block thread
	static void Loop(bool bQuitWhenIdle = false);
	static void LoopUntilIdle();

	static Looper* ThisLooper();

	// Á¢¼´ÍË³ö
	void QuitNow();
	void QuitWhenIdle();
	
	std::shared_ptr<MessageQueue> GetMessageQueue() { return message_queue_; };

	std::shared_ptr<TaskRunner> task_runner() { return task_runner_; }

	static std::shared_ptr<MessageQueue> CreateMessageQueueForType(LooperType loopType);

protected:
private:

	std::shared_ptr<MessageQueue> message_queue_;
	std::shared_ptr<TaskRunner> task_runner_;
};

#endif /* __LOOPER_H__ */