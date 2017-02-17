
#include "stdafx.h"
#include "animation.h"

using namespace graphic;

cAnimation::cAnimation()
	: m_skeleton(NULL)
	, m_isMergeAni(false)
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
		//m_isMergeAni = false;
		//m_anies.clear();
		//m_anies2.clear();
		//m_anies.reserve(anies.anies.size());
		//for (auto &ani : anies.anies)
		//	m_anies.push_back(cAnimationNode(&ani));
		m_anies.resize(1);
		for (auto &ani : anies.anies)
			m_anies[0].push_back(cAnimationNode(&ani));
	}

	m_skeleton = skeleton;

	return true;
}


bool cAnimation::Update(const float deltaSeconds)
{
	RETV(!m_skeleton, false);

	for (u_int k = 0; k < m_anies.size(); ++k)
	{
		auto &anies = m_anies[k];
		for (u_int i = 0; i < anies.size(); ++i)
		{
			Matrix44 result;
			if (anies[i].GetAnimationResult(deltaSeconds, result))
			{
				Vector3 p1 = m_skeleton->m_tmAni[i].GetPosition();
				Vector3 p2 = result.GetPosition();
				m_skeleton->m_tmAni[i].SetPosition( (p1+p2) * 0.5f );
				//m_skeleton->m_tmAni[i] = result;
			}
		}
	}

	//if (m_isMergeAni)
	//{
	//	for (u_int i = 0; i < m_anies2.size(); ++i)
	//	{
	//		Matrix44 result;
	//		if (m_anies2[i].GetAnimationResult(deltaSeconds, result))
	//		{
	//			Vector3 p1 = m_skeleton->m_tmAni[i].GetPosition();
	//			Vector3 p2 = result.GetPosition();
	//			m_skeleton->m_tmAni[i].SetPosition( (p1+p2) * 0.5f );
	//			
	//		}
	//	}
	//}

	m_skeleton->UpdateHierarcyTransform();

	return true;
}
