
#include "stdafx.h"
#include "model2.h"


using namespace graphic;


cModel2::cModel2()
	: m_mesh(NULL)
{
}

cModel2::~cModel2()
{
	Clear();
}


bool cModel2::Create(cRenderer &renderer, const string &modelName,
	MODEL_TYPE::TYPE type, const bool isLoadShader)
{
	sRawMeshGroup2 *rawMeshes = cResourceManager::Get()->LoadModel2(modelName);
	RETV(!rawMeshes, false);

	Clear();
	


	return true;
}


void cModel2::Clear()
{
	SAFE_DELETE(m_mesh);
}
