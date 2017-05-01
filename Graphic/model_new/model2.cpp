
#include "stdafx.h"
#include "model2.h"


using namespace graphic;

cModel2::cModel2()
	: m_colladaModel(NULL)
	, m_xModel(NULL)
	, m_shader(NULL)
{
}

cModel2::~cModel2()
{
	Clear();
}


// check collada file, xfile
bool cModel2::Create(cRenderer &renderer, const cFilePath &fileName)
{
	Clear();

	const string ext = fileName.GetFileExt();
	const bool isXFile =  (ext == ".x") || (ext == ".X");
	
	if (isXFile)
	{
		m_xModel = cResourceManager::Get()->LoadXFile(renderer, fileName.c_str());
		m_shader = cResourceManager::Get()->LoadShader(renderer, "hlsl_xfile.fx");
		if (m_shader)
			m_shader->SetTechnique("Scene_NoShadow");

		if (m_xModel)
		{
			m_boundingBox = m_xModel->m_boundingBox;
		}
	}
	else // Collada file
	{
		m_colladaModel = cResourceManager::Get()->LoadColladaModel(renderer, fileName.c_str());
		m_shader = cResourceManager::Get()->LoadShader(renderer, "hlsl_collada.fx");

		if (m_colladaModel)
		{
			m_animationName = m_colladaModel->m_storedAnimationName;
			m_boundingBox = m_colladaModel->m_boundingBox;
			m_shader->SetTechnique(m_colladaModel->m_isSkinning? "Skinning" : "Rigid");
		}
	}

	if (!m_xModel && !m_colladaModel)
		return false;

	m_fileName = fileName;

	return true;
}


bool cModel2::Render(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	// nothing~
	return true;
}


void cModel2::RenderShader(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	RET(!m_shader);

	GetMainCamera()->Bind(*m_shader);
	GetMainLight().Bind(*m_shader);

	const Matrix44 transform = m_tm * tm;

	if (m_colladaModel)
	{
		m_colladaModel->Render(renderer, *m_shader, transform);
	}
	else if (m_xModel)
	{
		m_xModel->RenderShader(renderer, *m_shader, transform);
	}

	//if (m_shader->m_isReload)
	//	m_shader->m_isReload = false;
}


bool cModel2::Update(const float deltaSeconds)
{
	bool reval = true;
	if (m_colladaModel)
	{
		reval = m_colladaModel->Update(deltaSeconds);
	}
	else if (m_xModel)
	{
		reval = m_xModel->Update(deltaSeconds);
	}
	return reval;
}


void cModel2::SetAnimation(const string &animationName
	, const bool isMerge //= false
)
{
	if (m_colladaModel)
		m_colladaModel->SetAnimation(animationName, isMerge);
}


void cModel2::Clear()
{
	m_colladaModel = NULL;
	m_xModel = NULL;
	m_shader = NULL;
}
