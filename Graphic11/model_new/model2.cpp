
#include "stdafx.h"
#include "model2.h"


using namespace graphic;

cModel2::cModel2()
	: cNode2(common::GenerateId(), "model", eNodeType::MODEL)
	, m_model(NULL)
	, m_state(eState::NORMAL)
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
	, const char *shaderName //= ""
	, const char *techniqueName //= "Unlit"
	, const bool isParallel //= false
	, const bool isShadow //= true
)
{
	Clear();

	m_id = id;
	m_name = fileName.GetFileNameExceptExt().c_str();
	m_fileName = fileName;
	m_shaderName = shaderName;
	m_techniqueName = techniqueName;
	SetRenderFlag(eRenderFlag::SHADOW, isShadow);

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
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);
	RETV(!m_model, false);

	const XMMATRIX transform = m_transform.GetMatrixXM() * parentTm;

	//const int vtxType = m_model->GetVertexType();
	//cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(vtxType);
	//assert(shader);
	//shader->SetTechnique(m_techniqueName.c_str());
	//shader->Begin();
	//shader->BeginPass(renderer, 0);
	//renderer.m_cbClipPlane.Update(renderer, 4);
	m_model->Render(renderer, m_techniqueName.c_str(), transform);

	//Transform tm2;
	//tm2.pos = m_transform.pos + tm.GetPosition();
	//tm2.pos.y += m_boundingBox.GetDimension().y * m_transform.scale.y + 5;
	//renderer.m_textMgr.AddTextRender(renderer, m_id, m_name.c_str()
	//	, cColor(1.f, 0.f, 0.f)
	//	, cColor(0.f, 0.f, 0.f)
	//	, BILLBOARD_TYPE::DYN_SCALE, tm2);

	__super::Render(renderer, parentTm, flags);
	return true;
}


bool cModel2::RenderInstancing(cRenderer &renderer
	, const int count
	, const XMMATRIX *transforms
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);

	const XMMATRIX transform = m_transform.GetMatrixXM() * parentTm;

	const Str32 technique = m_techniqueName + "_Instancing";
	//cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(eVertexType::POSITION | eVertexType::NORMAL | eVertexType::TEXTURE);
	//shader->SetTechnique(technique.c_str());
	//assert(shader);
	//shader->Begin();
	//shader->BeginPass(renderer, 0);
	//renderer.m_cbClipPlane.Update(renderer, 4);

	if (m_model)
		m_model->RenderInstancing(renderer, technique.c_str(), count, transforms, transform);

	//Transform tm2;
	//tm2.pos = m_transform.pos + tm.GetPosition();
	//tm2.pos.y += m_boundingBox.GetDimension().y * m_transform.scale.y + 5;
	//renderer.m_textMgr.AddTextRender(renderer, m_id, m_name.c_str()
	//	, cColor(1.f, 0.f, 0.f)
	//	, cColor(0.f, 0.f, 0.f)
	//	, BILLBOARD_TYPE::DYN_SCALE, tm2);

	__super::RenderInstancing(renderer, count, transforms, parentTm, flags);
	return true;
}


bool cModel2::Update(cRenderer &renderer, const float deltaSeconds)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);

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
			m_state = eState::LOAD_FINISH;
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
}


bool cModel2::IsLoadFinish()
{
	return (eState::LOAD_MESH_FINISH == m_state)
		|| (eState::LOAD_FINISH == m_state);
}

