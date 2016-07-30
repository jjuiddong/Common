//
// 2016-02-19, jjuiddong
//
//
// 전역 제어 객체
// MainLoop를 실행한다.
//
// Upate등록이 된 객체에게 Update() 함수를 호출한다.
//
#pragma once


#include "UpdateObserver.h"


class cController2 : public common::cSingleton < cController2 >
	, public cUpdateObservable
{
public:
	cController2();
	virtual ~cController2();

	bool Init();
	void Run();
	float GetGlobalSeconds() const;


	bool m_isLoop;
	float m_globalSeconds;
};


inline float cController2::GetGlobalSeconds() const { return m_globalSeconds; }
