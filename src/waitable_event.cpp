#include "waitable_event.h"

WaitableEvent::WaitableEvent()
	:signaled_(false)
{

}

WaitableEvent::~WaitableEvent()
{

}

void WaitableEvent::Signal()
{
	signaled_ = true;
	cv_.notify_one();
}

void WaitableEvent::Wait()
{
	std::unique_lock<std::mutex> lk(mutex_);
	while (!signaled_)
		cv_.wait(lk);
}
