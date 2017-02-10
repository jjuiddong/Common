#pragma once

#include "../action/actiondef.h"


namespace graphic
{

	class cCharacter : public cModel
								, public ai::iActorInterface<cCharacter>
	{
	public:
		cCharacter(const int id=-1);
		virtual ~cCharacter();

		virtual bool Update(const float elapseTime) override;
		virtual void Clear() override;

		void SetActionData(const vector<sActionData> &actions);
		void Action(const CHARACTER_ACTION::TYPE type, const int param1=0, const int param2=0);

		// ai interface implement
		virtual Matrix44& aiGetTransform() override;
		virtual void aiSetAnimation(const string &animationName) override;


	public:
		vector<sActionData> m_actions;
		CHARACTER_ACTION::TYPE m_action;
		//ai::cActor m_ai;
		string m_curAniName; // 플레이 중인 애니메이션.
	};

}
