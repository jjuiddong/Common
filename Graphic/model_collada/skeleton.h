//
// 2017-03-25, jjuiddong
//
#pragma once

namespace graphic
{

	class cSkeleton
	{
	public:
		cSkeleton();
		virtual ~cSkeleton();
		bool Create(const vector<sRawBone2> &bones);
		void UpdateHierarcyTransform();


	public:
		vector<Matrix44> m_tmOffset;
		vector<Matrix44> m_tmAni;
		vector<Matrix44> m_tmPose;
		vector<sRawBone2> m_bones;
	};

}