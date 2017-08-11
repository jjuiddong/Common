
#include "stdafx.h"
#include "model2.h"


using namespace graphic;

cModel2::cModel2()
	: cNode2(common::GenerateId(), "model", eNodeType::MODEL)
	, m_model(NULL)
	, m_state(eState::NORMAL)
	//, m_cullType(D3DCULL_CCW)
{
}

cModel2::~cModel2()
{
	Clear();
}


// check assimp file
bool cModel2::Create(cRenderer &renderer
	, const int id
	, const StrPath &fileName
	, const StrPath &shaderName //= ""
	, const Str32 &techniqueName //= ""
	, const bool isParallel //= false
	, const bool isShadow //= false
)
{
	Clear();

	m_id = id;
	m_name = fileName.GetFileNameExceptExt().c_str();
	m_fileName = fileName;
	m_shaderName = shaderName;
	m_techniqueName = techniqueName;
	m_isShadow = isShadow;
	m_flags |= eRenderFlag::SHADOW;

	if (isParallel)
	{
		m_state = eState::LOAD_PARALLEL;
		auto ret = cResourceManager::Get()->LoadAssimpModelParallel(renderer, fileName.c_str());
		m_model = ret.second;
	}
	else
	{
		m_state = eState::LOAD_SINGLE;
		m_model = cResourceManager::Get()->LoadAssimpModel(renderer, fileName.c_str());
	}

	if (!m_model)
		return false;

	return true;
}


bool cModel2::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(!m_isEnable, false);
	RETV(!m_isShow, false);
	//RETV(!m_shader, false);

	//renderer.SetCullMode(m_cullType);

	const XMMATRIX transform = m_transform.GetMatrixXM() * tm;

	//if (m_isShadow && m_shadowMap && m_shader)
	//	m_shadowMap->Bind(*m_shader, "g_shadowMapTexture");

	if (m_model)
		m_model->Render(renderer, transform);

	//Transform tm2;
	//tm2.pos = m_transform.pos + tm.GetPosition();
	//tm2.pos.y += m_boundingBox.GetDimension().y * m_transform.scale.y + 5;
	//renderer.m_textMgr.AddTextRender(renderer, m_id, m_name.c_str()
	//	, cColor(1.f, 0.f, 0.f)
	//	, cColor(0.f, 0.f, 0.f)
	//	, BILLBOARD_TYPE::DYN_SCALE, tm2);

	__super::Render(renderer, tm, flags);

	return true;
}


bool cModel2::Update(cRenderer &renderer, const float deltaSeconds)
{
	RETV(!m_isEnable, false);
	RETV(!m_isShow, false);

	if (CheckLoadProcess(renderer))
		return true;

	bool reval = true;
	if (m_model)
		reval = m_model->Update(deltaSeconds);

	__super::Update(renderer, deltaSeconds);

	return reval;
}


bool cModel2::CheckLoadProcess(cRenderer &renderer)
{
	if (eState::NORMAL == m_state)
		return false;

	switch (m_state)
	{
	case eState::LOAD_PARALLEL:
	{
		m_model = cResourceManager::Get()->FindAssimpModel(m_fileName.c_str()).second;
		if (m_model) // Parallel Load Finish
		{
			InitModel(renderer);

			if (m_isShadow)
			{
				m_state = eState::LOAD_MESH_FINISH;
			}
			else
			{
				m_state = eState::LOAD_FINISH;
			}
		}
	}
	break;

	case eState::LOAD_SINGLE:
	{
		InitModel(renderer);
		m_state = eState::LOAD_FINISH;
	}
	break;

	case eState::LOAD_MESH_FINISH:
		if (m_model)
			m_state = eState::LOAD_PARALLEL_SHADOW;
		break;

	case eState::LOAD_PARALLEL_SHADOW:
		m_state = eState::NORMAL; // no finish event
		break;

	case eState::LOAD_SINGLE_SHADOW:
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
	if (m_model)
	{
		m_animationName = m_model->m_storedAnimationName;
		m_boundingBox = m_model->m_boundingBox;
		m_boundingSphere.SetBoundingSphere(m_model->m_boundingBox);

		//SetShader( m_assimpModel->m_isSkinning ?
		//	cResourceManager::Get()->LoadShader(renderer, m_shaderName.empty() ? "assimp_skin.fx" : m_shaderName)
		//	: cResourceManager::Get()->LoadShader(renderer, m_shaderName.empty() ? "assimp_rigid.fx" : m_shaderName)
		//);

		//if (m_shader)
		//	m_shader->SetTechnique(m_techniqueName.empty() ? "Scene" : m_techniqueName);
		//if (m_shader)
		//	m_shader->SetTechnique("Scene");
			//m_shader->SetTechnique(m_assimpModel->m_isSkinning ? "Skinning" : "Rigid");
	}
}


void cModel2::SetAnimation(const Str64 &animationName
	, const bool isMerge //= false
)
{
	if (m_model)
		m_model->SetAnimation(animationName, isMerge);
}


void cModel2::Clear()
{
	m_model = NULL;
	//m_shader = NULL;
}


bool cModel2::IsLoadFinish()
{
	return (eState::LOAD_MESH_FINISH == m_state)
		|| (eState::LOAD_FINISH == m_state);
}


//void cModel2::SetShader(cShader *shader)
//{
//	__super::SetShader(shader);
//	__super::SetShadowShader(shader);
//}


void cModel2::UpdateShader(cRenderer &renderer)
{
	//RET(!m_shader);

	//GetMainCamera()->Bind(*m_shader);
	//GetMainLight().Bind(*m_shader);
}
