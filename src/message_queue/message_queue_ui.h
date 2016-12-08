/* -------------------------------------------------------------------------
//     filename :	message_queue_ui.h
//     author   :	cjzhou@tencent.cn
//     created  :	2016-4-11 20:57
//     purpose  :
//
// -----------------------------------------------------------------------*/
#ifndef __MESSAGE_QUEUE_UI_H__
#define __MESSAGE_QUEUE_UI_H__

#include <windows.h>
#include "message_queue.h"

class MessageQueueForUI : public MessageQueue
{
public:

	class MessageFilter
	{
	public:
		virtual ~MessageFilter() {}
		virtual BOOL DoPeekMessage(MSG* msg,
			HWND hWnd,
			UINT wMsgFilterMin,
			UINT wMsgFilterMax,
			UINT wRemoveMsg)
		{
			return PeekMessage(msg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
		}

		virtual bool ProcessMessage(const MSG& msg) { return false; }
	};

	MessageQueueForUI(bool bQuitAllowed);
	~MessageQueueForUI();

	void SetMessageFilter(std::shared_ptr<MessageFilter> filter);

	virtual void ScheduleWork();

	void PostQuit();

protected:
	// MessageQueue Methods
	virtual bool DoPriorityWork();
	virtual void TryWaitTask(const TimerTicks::duration& dura_time);

private:
	void InitMessageWnd();
	bool ProcessNextWindowsMessage();
	bool DoWindowsMessageHelper(const MSG& msg);
	bool DoReplacementMessage();
	
private:
	ATOM _atom;
	HWND message_hwnd_;
	std::shared_ptr<MessageFilter> _messageFilter;

	LONG have_work_;
};


#endif /* __MESSAGE_QUEUE_UI_H__ */