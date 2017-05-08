
#include "stdafx.h"
#include "model2.h"


using namespace graphic;

cModel2::cModel2()
	: m_colladaModel(NULL)
	, m_xModel(NULL)
	, m_state(eState::NORMAL)
	, m_shadow(NULL)
	, m_shadowMap(NULL)
	, m_isShadow(false)
	, m_isShadow2(false)
{
}

cModel2::~cModel2()
{
	Clear();
}


// check collada file, xfile
bool cModel2::Create(cRenderer &renderer, const cFilePath &fileName
	, const cFilePath &shaderName //= ""
	, const string &techniqueName //= ""
	, const bool isParallel //= false
	, const bool isShadow //= false
)
{
	Clear();

	m_fileName = fileName;
	m_shaderName = shaderName.c_str();
	m_techniqueName = techniqueName;
	m_isShadow = isShadow;

	const string ext = fileName.GetFileExt();
	const bool isXFile =  (ext == ".x") || (ext == ".X");

	if (isXFile) // Load XFile
	{
		if (isParallel)
		{
			m_state = eState::LOAD_PARALLEL_XFILE;
			auto ret = cResourceManager::Get()->LoadXFileParallel(renderer, fileName.c_str());
			m_xModel = ret.second;
		}
		else
		{
			m_state = eState::LOAD_SINGLE_XFILE;
			m_xModel = cResourceManager::Get()->LoadXFile(renderer, fileName.c_str());
		}
	}
	else // Load Collada file
	{
		if (isParallel)
		{
			m_state = eState::LOAD_PARALLEL_COLLADA;
			auto ret = cResourceManager::Get()->LoadColladaModelParallel(renderer, fileName.c_str());
			m_colladaModel = ret.second;
		}
		else
		{
			m_state = eState::LOAD_SINGLE_COLLADA;
			m_colladaModel = cResourceManager::Get()->LoadColladaModel(renderer, fileName.c_str());
		}
	}

	if (!m_xModel && !m_colladaModel)
		return false;

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

	const Matrix44 transform = m_tm * tm;

	if (m_isShadow2 && m_shadowMap && m_shader)
		m_shadowMap->Bind(*m_shader, "g_shadowMapTexture");

	if (m_colladaModel)
	{
		m_colladaModel->Render(renderer, *m_shader, transform);
	}
	else if (m_xModel)
	{
		m_xModel->RenderShader(renderer, *m_shader, transform);
	}
}


void cModel2::RenderShadow(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	RET(!m_shadow);
	RET(!m_shadowShader);

	m_shadowShader->SetMatrix("g_mWorld", m_tm * tm);
	
	const int pass = m_shadowShader->Begin();
	for (int i = 0; i < pass; ++i)
	{
		m_shadowShader->BeginPass(i);
		m_shadowShader->CommitChanges();
		m_shadow->Render(renderer);
		m_shadowShader->EndPass();
	}
	m_shadowShader->End();
}


bool cModel2::Update(cRenderer &renderer, const float deltaSeconds)
{
	if (CheckLoadProcess(renderer))
		return true;

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


bool cModel2::CheckLoadProcess(cRenderer &renderer)
{
	if (eState::NORMAL == m_state)
		return false;

	switch (m_state)
	{
	case eState::LOAD_PARALLEL_COLLADA:
	{
		m_colladaModel = cResourceManager::Get()->FindColladaModel(m_fileName.c_str()).second;
		if (m_colladaModel) // Parallel Load Finish
		{
			InitModel(renderer);

			if (m_isShadow)
			{
				cResourceManager::Get()->LoadShadowParallel(renderer, m_fileName.c_str());
				m_state = eState::LOAD_MESH_FINISH;
			}
			else
			{
				m_state = eState::LOAD_FINISH;
			}
		}
	}
	break;

	case eState::LOAD_PARALLEL_XFILE:
	{
		m_xModel = cResourceManager::Get()->FindXFile(m_fileName.c_str()).second;
		if (m_xModel) // Parallel Load Finish
		{
			InitModel(renderer);

			if (m_isShadow)
			{
				cResourceManager::Get()->LoadShadowParallel(renderer, m_fileName.c_str());
				m_state = eState::LOAD_MESH_FINISH;
			}
			else
			{
				m_state = eState::LOAD_FINISH;
			}
		}
	}
	break;

	case eState::LOAD_SINGLE_COLLADA:
	case eState::LOAD_SINGLE_XFILE:
	{
		InitModel(renderer);

		if (m_isShadow)
		{
			m_shadow = cResourceManager::Get()->LoadShadow(renderer, m_fileName.c_str());
		}

		m_state = eState::LOAD_FINISH;
	}
	break;

	case eState::LOAD_MESH_FINISH:
		if (m_colladaModel)
			m_state = eState::LOAD_PARALLEL_COLLADA_SHADOW;
		else if (m_xModel)
			m_state = eState::LOAD_PARALLEL_XFILE_SHADOW;
		break;

	case eState::LOAD_PARALLEL_COLLADA_SHADOW:
	case eState::LOAD_PARALLEL_XFILE_SHADOW:
	{
		m_shadow = cResourceManager::Get()->FindShadow(m_fileName.c_str()).second;
		if (m_shadow)
		{
			m_state = eState::NORMAL; // no finish event
		}
	}
	break;

	case eState::LOAD_SINGLE_COLLADA_SHADOW:
	case eState::LOAD_SINGLE_XFILE_SHADOW:
		break;

	case eState::LOAD_FINISH:
		m_state = eState::NORMAL;
		break;

	default: assert(0);
	}

	return true;
}


void cModel2::InitModel(cRenderer &renderer)
{
	if (m_colladaModel)
	{
		m_animationName = m_colladaModel->m_storedAnimationName;
		m_boundingBox = m_colladaModel->m_boundingBox;

		SetShader( m_colladaModel->m_isSkinning ?
			cResourceManager::Get()->LoadShader(renderer, m_shaderName.empty() ? "collada_skin.fx" : m_shaderName)
			: cResourceManager::Get()->LoadShader(renderer, m_shaderName.empty() ? "collada_rigid.fx" : m_shaderName)
		);

		if (m_shader)
			m_shader->SetTechnique(m_techniqueName.empty() ? "Scene" : m_techniqueName);
		if (m_shader)
			m_shader->SetTechnique("Scene");
			//m_shader->SetTechnique(m_colladaModel->m_isSkinning ? "Skinning" : "Rigid");
	}

	if (m_xModel)
	{
		m_boundingBox = m_xModel->m_boundingBox;

		SetShader(cResourceManager::Get()->LoadShader(renderer,
			m_shaderName.empty() ? "xfile.fx" : m_shaderName.c_str())
		);

		if (m_shader)
			m_shader->SetTechnique(m_techniqueName.empty() ? "Scene_NoShadow" : m_techniqueName);
	}
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
	m_shadow = NULL;
}


bool cModel2::IsLoadFinish()
{
	return (eState::LOAD_MESH_FINISH == m_state)
		|| (eState::LOAD_FINISH == m_state);
}


void cModel2::SetShader(cShader *shader)
{
	__super::SetShader(shader);
	__super::SetShadowShader(shader);
}
void cModel2::SetShadowShader(cShader *shader)
{
	__super::SetShader(shader);
	__super::SetShadowShader(shader);
}
