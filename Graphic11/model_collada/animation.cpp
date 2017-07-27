
#include "stdafx.h"
#include "animation.h"

using namespace graphic;

cAnimation::cAnimation()
	: m_skeleton(NULL)
	, m_isMergeAni(false)
	, m_state(eState::STOP)
	, m_incTime(0)
	, m_start(0)
	, m_end(0)
{
}

cAnimation::~cAnimation()
{
}


bool cAnimation::Create(const sRawAniGroup &anies, cSkeleton *skeleton, const bool isMerge)
// isMerge = false
{
	if (isMerge)
	{
		m_anies.push_back({});
		for (auto &ani : anies.anies)
			m_anies.back().push_back(cAnimationNode(&ani));
	}
	else
	{
		m_anies.resize(1);
		m_anies[0].clear();
		for (auto &ani : anies.anies)
			m_anies[0].push_back(cAnimationNode(&ani));
	}

	m_state = eState::PLAY;
	m_skeleton = skeleton;

	m_start = 0;
	m_end = 0;
	for (auto &ani : anies.anies)
	{
		if (ani.start != ani.end)
		{
			m_start = min(m_start, ani.start);
			m_end = max(m_end, ani.end);
		}
	}

	//m_anies[0].push_back(cAnimationNode(&ani));

	return true;
}


bool cAnimation::Update(const float deltaSeconds)
{
	RETV(!m_skeleton, false);
	RETV(m_state != eState::PLAY, false);
	
	m_incTime += deltaSeconds;
	if (m_incTime >= m_end)
		m_incTime = m_start;

	for (u_int k = 0; k < m_anies.size(); ++k)
	{
		auto &anies = m_anies[k];
		for (u_int i = 0; i < anies.size(); ++i)
		{
			Matrix44 result;
			if (anies[i].GetAnimationResult(m_incTime, result))
			{
				//Vector3 p1 = m_skeleton->m_tmAni[i].GetPosition();
				//Vector3 p2 = result.GetPosition();
				//Vector3 p3 = (p1 + p2) * 0.5f;
				//Matrix44 t;
				//t.SetPosition(p3);

				//Quaternion q1 = m_skeleton->m_tmAni[i].GetQuaternion();
				//Quaternion q2 = result.GetQuaternion();
				//Quaternion q3 = q1.Interpolate(q2, 0.5f);
				//Matrix44 r = q3.GetMatrix();

				m_skeleton->m_tmAni[i] = result;
				//m_skeleton->m_tmAni[i].SetPosition( (p1+p2) * 0.5f );
				//m_skeleton->m_tmAni[i].SetPosition(p2);
				//m_skeleton->m_tmAni[i] = result;
			}
		}
	}
	
	m_skeleton->UpdateHierarcyTransform();

	return true;
}


void cAnimation::Stop()
{
	m_state = eState::STOP;
}


void cAnimation::Play()
{
	m_state = eState::PLAY;
}
