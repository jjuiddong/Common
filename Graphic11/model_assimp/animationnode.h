//
// 2017-02-17, jjuiddong
// sBoneAni animation class 
//
#pragma once

namespace graphic
{
	
	class cAnimationNode
	{
	public:
		cAnimationNode(const sBoneAni *rawAni);
		virtual ~cAnimationNode();

		bool GetAnimationResult(const float curTime, OUT Matrix44 &out);
		bool GetAnimationResult(const float curTime, OUT Vector3 &pos, OUT Quaternion &rot, OUT Vector3 &scale);


	protected:
		bool GetPosKey(const float t, OUT Vector3 &out);
		bool GetRotKey(const float t, OUT Quaternion &out);
		bool GetScaleKey(const float t, OUT Vector3 &out);


	public:
		bool m_isEnable; // for debuggin
		const sBoneAni *m_boneAni; // reference
	};



	inline bool cAnimationNode::GetAnimationResult(const float curTime
		, OUT Vector3 &pos, OUT Quaternion &rot, OUT Vector3 &scale)
	{
		RETV(!m_isEnable, false);
		RETV(!m_boneAni, false);
		RETV(m_boneAni->start == m_boneAni->end, false);

		float t = curTime;
		if (m_boneAni->end < curTime)
			t = m_boneAni->end;

		GetPosKey(t, pos);
		GetRotKey(t, rot);
		scale = Vector3(1, 1, 1);
		GetScaleKey(t, scale);
		return true;
	}

}
