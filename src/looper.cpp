#include "looper.h"
#include "message_queue_ui.h"
#include <assert.h>

thread_local static Looper* sThreadLocal = NULL;

Looper::Looper(LooperType loopType /*= kLoopDefault*/)
	:_mQueue(CreateMessageQueueForType(loopType))
{

}

Looper::~Looper()
{

}

void Looper::Prepare(LooperType loopType /*= kLoopDefault*/)
{
	sThreadLocal = new Looper(loopType);
}

void Looper::Clean()
{
	if (sThreadLocal)
	{
		delete sThreadLocal;
		sThreadLocal = NULL;
	}
}

void Looper::Loop()
{
	std::shared_ptr<MessageQueue> mq = Looper::ThisLooper()->_mQueue;
	for (;;)
	{
		PendingTask pendingTask = mq->Next();
		if (!pendingTask.IsValid())
			return;

		pendingTask.task.Run();
	}

	Clean();
}

Looper* Looper::ThisLooper()
{
	return sThreadLocal;
}

static void QuitCurrentLooper()
{
	Looper* loop = Looper::ThisLooper();
	if (loop)
		loop->QuitNow();
}

static void QuitLooperWhenIdle()
{
	Looper* loop = Looper::ThisLooper();
	if (loop)
		loop->QuitWhenIdle();
}

Closure Looper::QuitClosure()
{
	return Bind(&QuitCurrentLooper);
}

Closure Looper::QuitWhenIdleClosure()
{
	return Bind(&QuitLooperWhenIdle);
}

void Looper::QuitNow()
{
	assert(this == ThisLooper());
	_mQueue->Quit();
}

void Looper::QuitWhenIdle()
{
	assert(this == ThisLooper());
	_mQueue->QuitWhenIdle();
}

std::shared_ptr<MessageQueue> Looper::CreateMessageQueueForType(LooperType loopType)
{
	if (loopType == kLoopDefault)
		return std::shared_ptr<MessageQueue>(new MessageQueue(false));
	else if (loopType == kLoopUI)
		return std::shared_ptr<MessageQueue>(new MessageQueueForUI(false));

	return nullptr;
}
