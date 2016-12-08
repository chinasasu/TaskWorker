#include "message_queue_ui.h"
#include "win/win_utils.h"
#include "base/string_helper.h"

static const wchar_t kWndClassNameFormat[] = L"TK_MessageQueueWindow_%p";
static const int kMsgHaveWorkID = WM_USER + 1;

#define HWND_MESSAGE     ((HWND)-3)

LRESULT CALLBACK WndProcThunk(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case kMsgHaveWorkID:
		reinterpret_cast<MessageQueueForUI*>(wparam)->ScheduleWork();
		break;
	case WM_TIMER:
		break;
	}

	return DefWindowProc(hwnd, message, wparam, lparam);
}



MessageQueueForUI::MessageQueueForUI(bool bQuitAllowed)
	: MessageQueue(bQuitAllowed),
	_atom(0),
	have_work_(0),
	_messageFilter(new MessageFilter)
{
	InitMessageWnd();
}

MessageQueueForUI::~MessageQueueForUI()
{
	DestroyWindow(message_hwnd_);
	UnregisterClass(MAKEINTATOM(_atom), GetModuleFromAddress(&WndProcThunk));
}

void MessageQueueForUI::SetMessageFilter(std::shared_ptr<MessageFilter> filter)
{
	_messageFilter = filter;
}

bool MessageQueueForUI::DoPriorityWork()
{
	// 处理窗口消息
	ProcessNextWindowsMessage();

	return false;
}

void MessageQueueForUI::TryWaitTask(const TimerTicks::duration& dura_time)
{
	if (InterlockedExchange(&have_work_, 0))
		return;

	int64_t delay = INFINITE;
	if (dura_time > TimerTicks::duration::zero())
		delay = std::chrono::duration_cast<std::chrono::milliseconds>(dura_time).count();

	DWORD result = MsgWaitForMultipleObjectsEx(0, NULL, (DWORD)delay, QS_ALLINPUT,
		MWMO_INPUTAVAILABLE);
	if (WAIT_OBJECT_0 == result)
	{
		MSG msg = { 0 };
		DWORD queue_status = GetQueueStatus(QS_MOUSE);
		if (HIWORD(queue_status) & QS_MOUSE &&
			!PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE)) 
		{
			WaitMessage();
		}

		return;
	}
	
	//TODO (wait failed!)
}

void MessageQueueForUI::InitMessageWnd()
{
	int error = 0;
	std::wstring className = StringPrintf(kWndClassNameFormat, this);

	HINSTANCE instance = GetModuleFromAddress(&WndProcThunk);

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WrappedWindowProc<WndProcThunk>;
	wc.hInstance = instance;
	wc.lpszClassName = className.c_str();
	_atom = RegisterClassEx(&wc);
	
  	message_hwnd_ = CreateWindow(MAKEINTATOM(_atom), 0, 0, 0, 0, 0, 0,
  								HWND_MESSAGE, 0, instance, 0);
}

bool MessageQueueForUI::ProcessNextWindowsMessage()
{
	bool sentMessage = false;
	DWORD status = GetQueueStatus(QS_SENDMESSAGE);
	if (HIWORD(status) & QS_SENDMESSAGE)
		sentMessage = true;
	
	MSG msg;
	if (_messageFilter->DoPeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != FALSE)
		return DoWindowsMessageHelper(msg);

	return sentMessage;
}

bool MessageQueueForUI::DoWindowsMessageHelper(const MSG& msg)
{
	if (WM_QUIT == msg.message)
	{
		Quit();
		return false;
	}
	
	if (msg.message == kMsgHaveWorkID && msg.hwnd == message_hwnd_)
		return DoReplacementMessage();

	::TranslateMessage(&msg);
	::DispatchMessage(&msg);

	return true;
}

bool MessageQueueForUI::DoReplacementMessage()
{
	MSG msg;
	bool haveMessage = !!_messageFilter->DoPeekMessage(&msg, NULL, 0, 0,
														PM_REMOVE);
	if (!haveMessage)
		return false;

	return DoWindowsMessageHelper(msg);
}

void MessageQueueForUI::ScheduleWork()
{
  	if (InterlockedExchange(&have_work_, 1))
  		return;  // 有可能已被激发过了

	BOOL ret = PostMessage(message_hwnd_, kMsgHaveWorkID,
		reinterpret_cast<WPARAM>(this), 0);
	int error = GetLastError();
	if (ret)
		return;  // There was room in the Window Message queue.

	// 好吧，如果到这里，表示PostMessage失败了
	// 清空参数先
	InterlockedExchange(&have_work_, 0);

	// TODO (PostMessage Failed!)
}

void MessageQueueForUI::PostQuit()
{
	PostMessage(message_hwnd_, WM_QUIT, reinterpret_cast<WPARAM>(this), 0);
}
