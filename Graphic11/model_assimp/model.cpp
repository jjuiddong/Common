
#include "stdafx.h"
#include "model.h"


using namespace graphic;

cModel::cModel()
	: cNode(common::GenerateId(), "model", eNodeType::MODEL)
	, m_model(NULL)
	, m_state(eState::NORMAL)
	, m_animationSpeed(1)
	, m_aniIncT(0)
{
}

cModel::~cModel()
{
	Clear();
}


// check assimp file
bool cModel::Create(cRenderer &renderer
	, const int id
	, const StrPath &fileName
	, const bool isParallel //= false
	, const bool isShadow //= true
)
{
	Clear();

	m_id = id;
	m_name = fileName.GetFileNameExceptExt().c_str();
	m_fileName = fileName;
	m_techniqueName = "Unlit";
	m_aniIncT = 0;
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


bool cModel::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	if (CheckLoadProcess(renderer))
		return true;

	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);
	RETV(!m_model, false);

	const XMMATRIX transform = m_transform.GetMatrixXM() * parentTm;

	m_model->Render(renderer, m_techniqueName.c_str(), transform);

	__super::Render(renderer, parentTm, flags);
	return true;
}


bool cModel::RenderInstancing(cRenderer &renderer
	, const int count
	, const XMMATRIX *transforms
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	if (CheckLoadProcess(renderer))
		return true;

	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);

	const XMMATRIX transform = m_transform.GetMatrixXM() * parentTm;

	const Str32 technique = m_techniqueName + "_Instancing";
	if (m_model)
		m_model->RenderInstancing(renderer, technique.c_str(), count, transforms, transform);

	__super::RenderInstancing(renderer, count, transforms, parentTm, flags);
	return true;
}


// return true if end of animation
bool cModel::Update(cRenderer &renderer, const float deltaSeconds)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);

	bool reval = false;
	m_aniIncT += deltaSeconds;
	if (m_model)
	{
		reval = m_model->Update(deltaSeconds*m_animationSpeed, m_aniIncT*m_animationSpeed);
		if (reval)
			m_aniIncT = 0;		
	}

	__super::Update(renderer, deltaSeconds);
	return reval;
}


bool cModel::CheckLoadProcess(cRenderer &renderer)
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


void cModel::InitModel(cRenderer &renderer)
{
	if (m_model)
	{
		m_animationName = m_model->m_storedAnimationName;
		m_boundingBox = m_model->m_boundingBox;
		m_boundingSphere.SetBoundingSphere(m_model->m_boundingBox);
	}
}


void cModel::SetAnimation(const Str64 &animationName
	, const bool isMerge //= false
)
{
	if (m_model)
		m_model->SetAnimation(animationName, isMerge);
}


void cModel::Clear()
{
	m_model = NULL;
}


bool cModel::IsLoadFinish()
{
	return (eState::LOAD_MESH_FINISH == m_state)
		|| (eState::LOAD_FINISH == m_state);
}


void cModel::Hilight(const bool isHilight)
{
	RET(!m_model);

	for (auto &mesh : m_model->m_meshes)
		mesh->m_isHilight = isHilight;
}
