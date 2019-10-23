//
// 2016-06-04, jjuiddong
//
//
#pragma once


namespace common
{

	class cTask;

	// 쓰레드와 로직간의 주고 받는 메세지 정의
	struct SExternalMsg
	{
		int rcvTaskId; // 메세지를 받을 Task ID (0=tread, -1=외부에서 받는 메세지)
		int msg;
		WPARAM wParam;
		LPARAM lParam;
		LPARAM added;

		SExternalMsg() {}
		SExternalMsg(int rcvtaskId, int msgtype, WPARAM wparam, LPARAM lparam, LPARAM _added) :
			rcvTaskId(rcvtaskId), msg(msgtype), wParam(wparam), lParam(lparam), added(_added)
		{
		}
	};

	
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
