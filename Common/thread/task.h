//------------------------------------------------------------------------
// Name:    Task.h
// Author:  jjuiddong
// Date:    2012-12-02
// 
// execute task in thread
//
// 2016-06-06
//		- refactoring
// 2017-05-03
//		- refactoring
//------------------------------------------------------------------------
#pragma once

#include "threaddef.h"


namespace common
{

	class cThread;
	class cTask
	{
	public:
		enum class eRunResult { End, Continue };		

		cTask(int id, const StrId &name="", const bool isTopPriority=false);
		virtual ~cTask() {}
		virtual eRunResult Run(const double deltaSeconds) { return eRunResult::End; }
		virtual void MessageProc( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added ) {}
		virtual void Clear() {}


	public:
		int m_id;
		bool m_isTopPriority;
		StrId m_name;
		cThread *m_pThread; // reference
	};

	inline cTask::cTask(int id, const StrId &name
		, const bool isTopPriority //= false
	) 
		: m_id(id), m_pThread(nullptr), m_name(name), m_isTopPriority(isTopPriority) { }


	// cTask search object
	class IsTask : public std::unary_function<cTask*, bool>
	{
	public:
		IsTask(int taskId):m_id(taskId) {}
		int m_id;
		bool operator ()(cTask *t) const
			{ return (t->m_id == m_id); }
	};

}
