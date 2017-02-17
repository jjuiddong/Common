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
		//const sRawAniGroup *m_rawAnies; // reference
		//const sRawAniGroup *m_rawAnies2; // reference
		cSkeleton *m_skeleton; // reference
		//vector<cAnimationNode> m_anies;
		//vector<cAnimationNode> m_anies2;
		
		vector<vector<cAnimationNode>> m_anies;

		bool m_isMergeAni;
	};

}