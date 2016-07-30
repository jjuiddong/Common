//
// 2016-04-13, jjuiddong
// ∏º« πÕΩÃ ¿Œ≈Õ∆‰¿ÃΩ∫
//
#pragma once

#include "motiondef.h"

namespace motion
{

	class cMixer
	{
	public:
		cMixer() : m_incTime(0), m_state(MODULE_STATE::STOP) {}
		virtual ~cMixer() {}

		virtual bool Start() = 0;
		virtual bool Stop() = 0;
		virtual bool Update(const float deltaSeconds) = 0;

	public:
		MODULE_STATE::TYPE m_state;
		float m_incTime;
	};

}
