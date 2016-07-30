//
// 2016-04-13, jjuiddong
// 모션 입력 인터페이스
//
#pragma once

#include "motiondef.h"

namespace motion
{

	class cInput
	{
	public:
		cInput(const MOTION_MEDIA::TYPE type) : m_type(type), m_state(MODULE_STATE::STOP), m_incTime(0) {}
		virtual ~cInput() {}

		virtual bool Start() = 0;
		virtual bool Stop() = 0;
		virtual bool Update(const float deltaSeconds) = 0;


	public:
		MOTION_MEDIA::TYPE m_type;
		MODULE_STATE::TYPE m_state;
		float m_incTime;
	};

}
