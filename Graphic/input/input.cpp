
#include "stdafx.h"
#include "input.h"

using namespace graphic;

cInputManager::cInputManager()
: m_lBtnDown(false)
, m_rBtnDown(false)
, m_mBtnDown(false)
{
	m_mousePt.x = 0;
	m_mousePt.y = 0;
}

cInputManager::~cInputManager()
{
}


void cInputManager::Update(const float deltaSeconds)
{
}


bool cInputManager::IsClick()
{
	return m_lBtnDown;
}
