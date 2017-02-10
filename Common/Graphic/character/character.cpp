
#include "stdafx.h"
#include "character.h"

using namespace graphic;


cCharacter::cCharacter(const int id) 
	: cModel((id==-1)? common::GenerateId() : id)
	, ai::iActorInterface<cCharacter>(this)
	, m_action(CHARACTER_ACTION::NONE)
//, m_ai(this)
{
}

cCharacter::~cCharacter()
{
	Clear();
}


// 캐릭터에게 명령을 내린다.
void cCharacter::Action(const CHARACTER_ACTION::TYPE type, const int param1, const int param2)
	// param1=0, param2 = 0
{
	RET(m_action == type);

	for each (auto &action in m_actions)
	{
		if (action.type == type)
		{
			m_action = type;
			SetAnimation(action.animationFile, 0, true);
			break;
		}
	}
}

// 
void cCharacter::SetActionData(const vector<sActionData> &actions)
{
	m_actions = actions;
}


bool cCharacter::Update(const float deltaSeconds) 
{
	cModel::Update(deltaSeconds);

	//m_ai.Update(deltaSeconds);

	return true;
}



Matrix44& cCharacter::aiGetTransform() 
{
	return m_TM; 
}


void cCharacter::aiSetAnimation(const string &animationName)
{
	RET(animationName.empty());

	// 같은 애니메이션이라면 다시 플레이하지 않는다.
	if (m_curAniName != animationName)
	{
		SetAnimation(animationName, 0, true);
		m_curAniName = animationName;
	}
}


void cCharacter::Clear()
{
	cModel::Clear();
}
