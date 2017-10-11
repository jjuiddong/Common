//
// 2017-03-25, jjuiddong
//
#pragma once


namespace graphic
{

	class cAnimation
	{
	public:
		cAnimation();
		virtual ~cAnimation();

		bool Create(cSkeleton *skeleton);
		bool Create(const sRawAniGroup &anies, cSkeleton *skeleton, const bool isMerge=false);
		bool Update(const float deltaSeconds);
		void Stop();
		void Play();


	public:
		struct eState {enum Enum {STOP,PLAY,};};

		eState::Enum m_state;
		cSkeleton *m_skeleton; // reference
		vector<vector<cAnimationNode>> m_anies;
		bool m_isMergeAni;
		float m_start;
		float m_end;
		float m_incTime;
	};

}