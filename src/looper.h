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
#include "callback/bind.h"
#include "message_queue.h"

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
	static void Clean();

	// block thread
	static void Loop();

	static Looper* ThisLooper();

	static Closure QuitClosure();
	static Closure QuitWhenIdleClosure();
	
	// Á¢¼´ÍË³ö
	void QuitNow();
	void QuitWhenIdle();
	
	MessageQueue* GetMessageQueue() { return _mQueue.get(); };

	static std::shared_ptr<MessageQueue> CreateMessageQueueForType(LooperType loopType);

protected:
private:

	std::shared_ptr<MessageQueue> _mQueue;
};

#endif /* __LOOPER_H__ */