
#include "stdafx.h"
#include "animationnode.h"

using namespace graphic;


cAnimationNode::cAnimationNode(const sBoneAni *boneAni)
	: m_boneAni(boneAni)
{
}

cAnimationNode::~cAnimationNode()
{
}


bool cAnimationNode::GetAnimationResult(const float curTime, OUT Matrix44 &out)
{
	Vector3 pos(0, 0, 0);
	Quaternion q(0, 0, 0, 1);
	Vector3 scale(1, 1, 1);
	if (!GetAnimationResult(curTime, pos, q, scale))
		return false;

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
	for (int i = 0; i < (int)m_boneAni->pos.size(); ++i)
	{
		if (m_boneAni->pos[i].t >= t)
		{
			const int prevIdx = (i > 0)? i - 1 : 0;
			const sKeyPos &prev = m_boneAni->pos[prevIdx];

			const float totTime = m_boneAni->pos[i].t - prev.t;
			const float curTime = t - prev.t;
			const float r = (totTime > 0) ? min(1, curTime / totTime) : 1.f;
			out = prev.p.Interpolate(m_boneAni->pos[i].p, r);
			return true;
		}
	}

	out = (m_boneAni->pos.empty()) ? Vector3(0, 0, 0) : m_boneAni->pos.back().p;
	return true;
}


bool cAnimationNode::GetRotKey(const float t, OUT Quaternion &out)
{
	for (int i = 0; i < (int)m_boneAni->rot.size(); ++i)
	{
		if (m_boneAni->rot[i].t >= t)
		{
			const int prevIdx = (i > 0) ? i - 1 : 0;
			const sKeyRot &prev = m_boneAni->rot[prevIdx];

			const float totTime = m_boneAni->rot[i].t - prev.t;
			const float curTime = t - prev.t;
			const float r = (totTime > 0) ? min(1, curTime / totTime) : 1.f;
			out = prev.q.Interpolate(m_boneAni->rot[i].q, r);
			return true;
		}
	}

	out = (m_boneAni->rot.empty()) ? Quaternion(0, 0, 0, 1) : m_boneAni->rot.back().q;
	return true;
}


bool cAnimationNode::GetScaleKey(const float t, OUT Vector3 &out)
{
	for (int i = 0; i < (int)m_boneAni->scale.size(); ++i)
	{
		if (m_boneAni->scale[i].t >= t)
		{
			const int prevIdx = (i > 0) ? i - 1 : 0;
			const sKeyScale &prev = m_boneAni->scale[prevIdx];

			const float totTime = m_boneAni->scale[i].t - prev.t;
			const float curTime = t - prev.t;
			const float r = (totTime > 0) ? min(1, curTime / totTime) : 1.f;
			out = prev.s.Interpolate(m_boneAni->scale[i].s, r);
			return true;
		}
	}

	out = (m_boneAni->scale.empty()) ? Vector3(1, 1, 1) : m_boneAni->scale.back().s;
	return true;
}
