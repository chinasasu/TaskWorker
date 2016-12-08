#include "looper.h"

#include <assert.h>
#include "message_queue/message_queue.h"
#include "message_queue/message_queue_ui.h"

thread_local static Looper* sThreadLocal = NULL;

Looper::Looper(LooperType loopType /*= kLoopDefault*/)
	:_mQueue(CreateMessageQueueForType(loopType))
{
	task_runner_.reset(new TaskRunner(_mQueue));
}

Looper::~Looper()
{

}

void Looper::Prepare(LooperType loopType /*= kLoopDefault*/)
{
	sThreadLocal = new Looper(loopType);
}

void Looper::Reset()
{
	if (sThreadLocal)
	{
		sThreadLocal->GetMessageQueue()->ResetQuit();
	}
}

void Looper::Clean()
{
	if (sThreadLocal)
	{
		delete sThreadLocal;
		sThreadLocal = NULL;
	}
}

void Looper::Loop(bool bQuitWhenIdle /*= false*/)
{
	std::shared_ptr<MessageQueue> mq = Looper::ThisLooper()->_mQueue;
	if (bQuitWhenIdle)
		mq->QuitWhenIdle();

	for (;;)
	{
		PendingTask pendingTask = mq->Next();
		if (!pendingTask.IsValid())
			break;

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
