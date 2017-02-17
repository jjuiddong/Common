
#include "stdafx.h"
#include "animationnode.h"

using namespace graphic;


cAnimationNode::cAnimationNode(const sRawAni *rawAni)
	: m_incTime(0)
	, m_rawAni(rawAni)
{
}

cAnimationNode::~cAnimationNode()
{
}


bool cAnimationNode::GetAnimationResult(const float deltaSeconds, OUT Matrix44 &out)
{
	RETV(!m_rawAni, false);
	RETV(m_rawAni->start == m_rawAni->end, false);

	m_incTime += deltaSeconds;

	if (m_rawAni->end < m_incTime)
		m_incTime = m_rawAni->start;

	Vector3 pos(0,0,0);
	GetPosKey(m_incTime, pos);

	Quaternion q(0,0,0,1);
	GetRotKey(m_incTime, q);

	Vector3 scale(1,1,1);
	GetScaleKey(m_incTime, scale);

	Matrix44 T;
	T.SetPosition(pos);
	Matrix44 R = q.GetMatrix();
	Matrix44 S;
	S.SetScale(scale);

	//out  = R * S * T;
	out = S * R * T;

	return true;
}


bool cAnimationNode::GetPosKey(const float t, OUT Vector3 &out)
{
	RETV(m_rawAni->pos.empty(), true);

	for (u_int i = 0; i < m_rawAni->pos.size(); ++i)
	{
		if (m_rawAni->pos[i].t >= t)
		{
			out = m_rawAni->pos[i].p;
			return true;
		}
	}

	out = (m_rawAni->pos.empty()) ? Vector3(0, 0, 0) : m_rawAni->pos[0].p;

	return true;
}


bool cAnimationNode::GetRotKey(const float t, OUT Quaternion &out)
{
	RETV(m_rawAni->rot.empty(), true);

	for (u_int i = 0; i < m_rawAni->rot.size(); ++i)
	{
		if (m_rawAni->rot[i].t >= t)
		{
			out = m_rawAni->rot[i].q;
			return true;
		}
	}

	out = (m_rawAni->rot.empty()) ? Quaternion(0, 0, 0, 1) : m_rawAni->rot[0].q;

	return true;
}


bool cAnimationNode::GetScaleKey(const float t, OUT Vector3 &out)
{
	RETV(m_rawAni->scale.empty(), true);

	for (u_int i = 0; i < m_rawAni->scale.size(); ++i)
	{
		if (m_rawAni->scale[i].t >= t)
		{
			out = m_rawAni->scale[i].s;
			return true;
		}
	}

	out = (m_rawAni->scale.empty()) ? Vector3(0, 0, 0) : m_rawAni->scale[0].s;

	return true;
}
