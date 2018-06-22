//------------------------------------------------------------------------
// Name:    Task.h
// Author:  jjuiddong
// Date:    2012-12-02
// 
// 쓰레드에서 실행되는 태스크를 정의한다.
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
		struct eRunResult {
			enum Enum { END, CONTINUE };
		};

		cTask(int id, const std::string &name="", const bool isTopPriority=false);
		virtual ~cTask() {}
		virtual eRunResult::Enum Run(const double deltaSeconds) { return eRunResult::END; }
		virtual void MessageProc( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added ) {}
		virtual void Clear() {}


	public:
		int m_id;
		bool m_isTopPriority;
		cThread *m_pThread;
		std::string m_name;
	};

	inline cTask::cTask(int id, const std::string &name
		, const bool isTopPriority //= false
	) 
		: m_id(id), m_pThread(NULL), m_name(name), m_isTopPriority(isTopPriority) { }


	// list<CTask*>에서 CTask를 찾는 객체
	class IsTask : public std::unary_function<cTask*, bool>
	{
	public:
		IsTask(int taskId):m_id(taskId) {}
		int m_id;
		bool operator ()(cTask *t) const
			{ return (t->m_id == m_id); }
	};

}
