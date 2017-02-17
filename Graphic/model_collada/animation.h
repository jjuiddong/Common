#pragma once


namespace graphic
{

	class cAnimation
	{
	public:
		cAnimation();
		virtual ~cAnimation();

		bool Create(const sRawAniGroup &anies, cSkeleton *skeleton, const bool isMerge=false);
		bool Update(const float deltaSeconds);


	public:
		cSkeleton *m_skeleton; // reference
		vector<vector<cAnimationNode>> m_anies;

		bool m_isMergeAni;
	};

}