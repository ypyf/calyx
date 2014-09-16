#include "event.h"

using namespace calyx::event;

void EventQueue::PostEvent(int event)
{
	m_queue.push(event);
}

int EventQueue::PumpEvent()
{
	m_queue.pop();
	//static MSG msg;
	//if (!m_queue.empty()) {
	//	if (msg.message == WM_QUIT) {
	//		exit((int)msg.wParam);
	//	}
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}
	//else {
	//	if (gut_on_idle)
	//		gut_on_idle();
	//}
	return 0;
}

void EventQueue::DispatchEvent(int event)
{
	//switch (event)
	//{
	//case EVENT_QUIT:
	//	::PostQuitMessage(0);
	//	break;
	//case EVENT_PAUSE:
	//	m_bSuspended = true;
	//	break;
	//case EVENT_RESUME:
	//	m_bSuspended = false;
	//	break;
	//case EVENT_FOCUS:
	//	m_bFocus = true;
	//	break;
	//default: break;
	//}
}