//
// 2016-06-04, jjuiddong
//
//
#pragma once


namespace common
{

	class cTask;
	typedef std::list<cTask*> TaskList;
	typedef TaskList::iterator TaskItor;

	// 쓰레드와 로직간의 주고 받는 메세지 정의
	typedef struct _SExternalMsg
	{
		int rcvTaskId;			// 메세지를 받을 Task ID (0=tread, -1=외부에서 받는 메세지)
		int msg;
		WPARAM wParam;
		LPARAM lParam;
		LPARAM added;

		_SExternalMsg() {}
		_SExternalMsg(int rcvtaskId, int msgtype, WPARAM wparam, LPARAM lparam, LPARAM _added) :
			rcvTaskId(rcvtaskId), msg(msgtype), wParam(wparam), lParam(lparam), added(_added)
		{
		}

	} SExternalMsg;

	typedef std::list<SExternalMsg> ExternalMsgList;
	typedef ExternalMsgList::iterator ExternalMsgItor;

	
	namespace threadmsg
	{
		enum MSG
		{
			TASK_MSG = 100,// task message
										// wParam : taskId
			TERMINATE_TASK,
										// wParam : taskId
			MSG_LAST
		};
	
	}
}
