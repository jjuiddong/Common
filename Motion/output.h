//
// 모션 출력 인터페이스 
//
//
#pragma once

#include "motiondef.h"


namespace motion
{

	class cOutput
	{
	public:
		cOutput(const MOTION_MEDIA::TYPE type) : m_type(type), m_state(MODULE_STATE::STOP), m_incTime(0), m_formatIdx(0){}
		virtual ~cOutput() {}

		virtual bool Start() = 0;
		virtual bool Stop() = 0;
		virtual bool Update(const float deltaSeconds) = 0;
		virtual void SendImmediate() {}
		virtual void SetFormat(const int index) { m_formatIdx = index; }


	public:
		MOTION_MEDIA::TYPE m_type;
		MODULE_STATE::TYPE m_state;
		float m_incTime;
		int m_formatIdx;
	};

}
