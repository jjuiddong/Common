
#include "stdafx.h"
#include "model.h"


using namespace graphic;

cModel::cModel()
	: cNode(common::GenerateId(), "model", eNodeType::MODEL)
	, m_model(NULL)
	, m_state(eState::NORMAL)
	, m_animationSpeed(1)
	, m_aniIncT(0)
	, m_isLoad(false)
{
	SetRenderFlag(eRenderFlag::NOALPHABLEND, false);
	SetRenderFlag(eRenderFlag::ALPHABLEND, true);
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
	m_isLoad = false;
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

	if (!(flags & m_renderFlags))
		return false;

	if (IsRenderFlag(eRenderFlag::ALPHABLEND))
	{
		CommonStates state(renderer.GetDevice());
		renderer.GetDevContext()->OMSetBlendState(state.NonPremultiplied(), NULL, 0xffffffff);
		const XMMATRIX transform = m_transform.GetMatrixXM() * parentTm;
		m_model->Render(renderer, m_techniqueName.c_str(), &m_skeleton, transform);
		renderer.GetDevContext()->OMSetBlendState(NULL, NULL, 0xffffffff);
	}
	else
	{
		const XMMATRIX transform = m_transform.GetMatrixXM() * parentTm;
		m_model->Render(renderer, m_techniqueName.c_str(), &m_skeleton, transform);
	}

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
		m_model->RenderInstancing(renderer, technique.c_str(), &m_skeleton, count, transforms, transform);

	__super::RenderInstancing(renderer, count, transforms, parentTm, flags);
	return true;
}


// return true if end of animation
bool cModel::Update(cRenderer &renderer, const float deltaSeconds)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);

	bool reval = false;
	const float dt = deltaSeconds * m_animationSpeed;
	m_aniIncT += dt;
	if (m_model)
	{
		m_model->Update(deltaSeconds);
		reval = m_animation.Update(m_aniIncT);

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
	RET(!m_model);

	if (m_animationName.empty()) // 애니메이션이 설정된 상태라면, 덮어 씌우지 않는다.
		m_animationName = m_model->m_storedAnimationName;

	m_isLoad = true;
	m_boundingBox = m_model->m_boundingBox;
	m_boundingSphere.SetBoundingSphere(m_model->m_boundingBox);

	m_skeleton = m_model->m_skeleton;
	const int curAniIdx = m_animation.m_curAniIdx;
	m_animation = m_model->m_animation;
	m_animation.m_skeleton = &m_skeleton;

	// 모델이 로딩 되기전에 애니메이션 정보를 설정했다면, 그대로 유지한다. (병렬로 로딩 되었을 경우)
	if (curAniIdx >= 0)
		m_animation.m_curAniIdx = curAniIdx;
	else
		SetAnimation(m_animationName);
}


void cModel::SetAnimation(const Str64 &animationName
	, const bool isMerge //= false
)
{
	m_animationName = animationName;
	if (m_animation.SetAnimation(animationName, isMerge))
		m_animation.Play();
}


void cModel::SetAnimation(const int animationIndex
	, const bool isMerge //= false
)
{
	if (m_animation.SetAnimation(animationIndex, isMerge))
		m_animation.Play();
}


cNode* cModel::Clone(cRenderer &renderer) const
{
	cModel *model = new cModel();
	const int id = model->m_id;
	*model = *this;
	model->m_id = id;
	return model;
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
