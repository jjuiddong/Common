
#include "stdafx.h"
#include "aimain.h"

using namespace ai;


// AI 전역 매인 루프 함수
void ai::Loop(const float deltaSeconds)
{
	cMessageManager::Get()->DispatchMsg();
}


// AI 라이브러리 초기화
void ai::Clear()
{
	cMessageManager::Release();
}
