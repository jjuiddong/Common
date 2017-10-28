
#include "stdafx.h"
#include "animation.h"

using namespace graphic;


cAnimation::cAnimation()
	: m_skeleton(NULL)
	, m_curAniIdx(-1)
	, m_isMergeAni(false)
	, m_state(eState::STOP)
	, m_start(0)
	, m_end(0)
	, m_aniGroup(NULL)
{
}

cAnimation::~cAnimation()
{
}


// Empty Animation
bool cAnimation::Create(cSkeleton *skeleton)
{
	m_state = eState::PLAY;
	m_skeleton = skeleton;
	return true;
}


bool cAnimation::Create(const sRawAniGroup &aniGroup, cSkeleton *skeleton
	, const bool isMerge //= false
)
{
	if (isMerge)
	{
		assert(0);
	}
	else
	{
		m_curAniIdx = 0;
		m_anies.reserve(aniGroup.anies.size());
		for (auto &srcAni : aniGroup.anies)
		{
			m_anies.push_back({});
			vector<cAnimationNode> &dst = m_anies.back();
			dst.reserve(srcAni.boneAnies.size());

			for (auto &boneAni : srcAni.boneAnies)
				dst.push_back(cAnimationNode(&boneAni));
		}
	}

	m_aniGroup = &aniGroup;
	m_state = eState::PLAY;
	m_skeleton = skeleton;

	if (m_curAniIdx >= 0)
	{
		m_start = aniGroup.anies[m_curAniIdx].start;
		m_end = aniGroup.anies[m_curAniIdx].end;
	}
	else
	{
		m_start = 0;
		m_end = 0;
	}

	return true;
}


// return true, if end of animation 
bool cAnimation::Update(const float increasedTime)
{
	RETV(!m_skeleton, false);
	RETV(m_state != eState::PLAY, false);
	
	bool isEnd = false;
	float t = increasedTime;
	if (t >= m_end)
	{
		t = m_end;
		isEnd = true;
	}

	if (m_curAniIdx >= 0)
	{
		auto &ani = m_anies[m_curAniIdx];
		for (u_int i = 0; i < ani.size(); ++i)
		{
			Matrix44 result;
			if (ani[i].GetAnimationResult(t, result))
				m_skeleton->m_tmAni[i] = result;
		}
	}
	
	m_skeleton->UpdateHierarchyTransform();

	return isEnd;
}


bool cAnimation::SetAnimation(const StrId &animationName
	, const bool isMerge //= false
)
{
	RETV(!m_aniGroup, false);

	for (u_int i = 0; i < m_aniGroup->anies.size(); ++i)
	{
		if (m_aniGroup->anies[i].name == animationName)
		{
			m_curAniIdx = i;
			m_start = m_aniGroup->anies[i].start;
			m_end = m_aniGroup->anies[i].end;
			return true;
		}
	}

	return false;
}


bool cAnimation::SetAnimation(const int animationIndex
	, const bool isMerge //= false
)
{
	RETV(!m_aniGroup, false);
	RETV((int)m_aniGroup->anies.size() <= animationIndex, false);

	m_curAniIdx = animationIndex;
	m_start = m_aniGroup->anies[animationIndex].start;
	m_end = m_aniGroup->anies[animationIndex].end;
	return true;
}


void cAnimation::Stop()
{
	m_state = eState::STOP;
	m_curAniIdx = -1;
	if (m_skeleton)
	{
		m_skeleton->SetInitializePose();
		m_skeleton->UpdateHierarchyTransform();
	}
}


void cAnimation::Play()
{
	m_state = eState::PLAY;
}
