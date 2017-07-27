//
// 2017-02-17, jjuiddong
// sRawAni animation class 
//
#pragma once

namespace graphic
{
	
	class cAnimationNode
	{
	public:
		cAnimationNode(const sRawAni *rawAni);
		virtual ~cAnimationNode();

		bool GetAnimationResult(const float curTime, OUT Matrix44 &out);


	protected:
		bool GetPosKey(const float t, OUT Vector3 &out);
		bool GetRotKey(const float t, OUT Quaternion &out);
		bool GetScaleKey(const float t, OUT Vector3 &out);


	public:
		const sRawAni *m_rawAni; // reference
	};

}
