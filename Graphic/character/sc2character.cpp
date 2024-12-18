
#include "stdafx.h"
#include "character.h"
#include "sc2character.h"

using namespace graphic;


cSc2Character::cSc2Character(const int id)
	: cCharacter(id)
{
}

cSc2Character::~cSc2Character()
{
}


// 캐릭터 모델 생성.
bool cSc2Character::Create(cRenderer &renderer, const StrPath &modelName, MODEL_TYPE::TYPE type
	, const bool isLoadShader)
// type = MODEL_TYPE::AUTO
// isLoadShader = true;
{
	if (cModel::Create(renderer, modelName, type, isLoadShader))
	{
		if (GetModelType() == MODEL_TYPE::SKIN)
		{
			SetShader( 
				cResourceManager::Get()->LoadShader(renderer, "hlsl_skinning_using_texcoord_sc2.fx") );
		}
		return true;
	}

	return true;
}

