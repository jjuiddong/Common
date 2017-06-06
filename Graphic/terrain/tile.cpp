
#include "stdafx.h"
#include "tile.h"

using namespace graphic;


cTile::cTile()
	: m_isShadow(true)
	, m_isShow(true)
	, m_isDbgRender(false)
{
}

cTile::~cTile()
{
	Clear();
}


bool cTile::Create(cRenderer &renderer
	, const string &name
	, const sRectf &rect
	, const float y //=0
	, const float uvFactor //= 1.f,
	, const Vector2 &uv0 //= Vector2(0, 0)
	, const Vector2 &uv1 //= Vector2(1, 1)
)
{
	m_name = name;

	const float cellSize = rect.Width() / 2.f;
	m_ground.Create(renderer, 2, 2, cellSize, 1, y, uv0, uv1);
	m_ground.SetShader(cResourceManager::Get()->LoadShader(renderer, "tile.fx"));
	m_ground.m_shader->SetFloat("g_uvFactor", uvFactor);
	m_ground.m_mtrl.InitXFile();
	
	Matrix44 T;
	T.SetPosition(Vector3(rect.left+cellSize, 0, rect.top + cellSize));
	m_tm = T;

	m_boundingBox.SetBoundingBox(Vector3(rect.left, 0, rect.top)
		, Vector3(rect.right, 20, rect.bottom));

	m_dbgTile.SetBox(renderer, Vector3(rect.left, 0, rect.top)
		, Vector3(rect.right, 20, rect.bottom));

	return true;
}


void cTile::Update(cRenderer &renderer, const float deltaSeconds)
{
	for (auto &p : m_models)
		p->Update(renderer, deltaSeconds);
}


void cTile::PreRender(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	RET(!m_isShadow);
	RET(!m_isShow);

	cCamera *cam = GetMainCamera();

	for (auto &shader : m_shaders)
	{
		shader->SetTechnique("ShadowMap");
		shader->SetMatrix("g_mView", cam->GetViewMatrix());
		shader->SetMatrix("g_mProj", cam->GetProjectionMatrix());
	}

	for (auto &p : m_models)
	{
		if (!p->m_isShadow)
			continue;
		if (p->m_isShow)
  			p->RenderShader(renderer);
	}
}


void cTile::Render(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	RET(!m_isShow);

	m_ground.m_shader->SetTechnique("Scene_NoShadow");

	m_ground.RenderShader(renderer, m_tm * tm);

	for (auto &shader : m_shaders)
		shader->SetTechnique("Scene_NoShadow");

	for (auto &p : m_models)
		if (p->m_isShow)
			p->RenderShader(renderer, tm);

	if (m_isDbgRender)
		m_dbgTile.Render(renderer, tm);
}


void cTile::Render( cRenderer &renderer
	, const Matrix44 &mVPT, const Matrix44 &mLVP 
	, cShadowMap *shadowMap //= NULL
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	RET(!m_isShow);

	if (m_isShadow && shadowMap)
	{
		m_ground.m_shader->SetTechnique("Scene_ShadowMap");
		m_ground.m_shader->SetMatrix("g_mVPT", mVPT);
		m_ground.m_shader->SetMatrix("g_mLVP", mLVP);
		shadowMap->Bind(*m_ground.m_shader, "g_shadowMapTexture");
	}
	else
	{
		m_ground.m_shader->SetTechnique("Scene_NoShadow");
	}

	m_ground.RenderShader(renderer, m_tm * tm);

	for (auto &shader : m_shaders)
	{
		if (m_isShadow && shadowMap)
		{
			shader->SetTechnique("Scene_ShadowMap");
			shader->SetMatrix("g_mVPT", mVPT);
			shader->SetMatrix("g_mLVP", mLVP);
			shadowMap->Bind(*shader, "g_shadowMapTexture");
		}
		else
		{
			shader->SetTechnique("Scene_NoShadow");
		}
	}

	for (auto &p : m_models)
		if (p->m_isShow)
			p->RenderShader(renderer, tm);

	if (m_isDbgRender)
		m_dbgTile.Render(renderer, tm);
}


void cTile::Render2(cRenderer &renderer
	, const Matrix44 &mLightView, const Matrix44 &mLightProj, const Matrix44 &mLightTT
	, cShadowMap *shadowMap //= NULL
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	RET(!m_isShow);

	if (m_isShadow && shadowMap)
	{
		m_ground.m_shader->SetTechnique("Scene_ShadowMap");
		m_ground.m_shader->SetMatrix("g_mLightView1", mLightView);
		m_ground.m_shader->SetMatrix("g_mLightProj1", mLightProj);
		m_ground.m_shader->SetMatrix("g_mLightTT1", mLightTT);
		shadowMap->Bind(*m_ground.m_shader, "g_shadowMapTexture1");
	}
	else
	{
		m_ground.m_shader->SetTechnique("Scene_NoShadow");
	}

	m_ground.RenderShader(renderer, m_tm * tm);

	for (auto &shader : m_shaders)
	{
		if (m_isShadow && shadowMap)
		{
			shader->SetTechnique("Scene_ShadowMap");
			shader->SetMatrix("g_mLightView1", mLightView);
			shader->SetMatrix("g_mLightProj1", mLightProj);
			shader->SetMatrix("g_mLightTT1", mLightTT);
			shadowMap->Bind(*shader, "g_shadowMapTexture1");
		}
		else
		{
			shader->SetTechnique("Scene_NoShadow");
		}
	}

	for (auto &p : m_models)
		if (p->m_isShow)
			p->RenderShader(renderer, tm);

	if (m_isDbgRender)
		m_dbgTile.Render(renderer, tm);
}


void cTile::Render2(cRenderer &renderer
	, const Matrix44 *mLightView, const Matrix44 *mLightProj, const Matrix44 *mLightTT,
	cShadowMap *shadowMap, const int shadowMapCount
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	RET(!m_isShow);

	char *varShadowMap[] = { "g_shadowMapTexture1", "g_shadowMapTexture2", "g_shadowMapTexture3" };
	char *varLightView[] = { "g_mLightView1", "g_mLightView2", "g_mLightView3" };
	char *varLightProj[] = { "g_mLightProj1", "g_mLightProj2", "g_mLightProj3" };
	char *varLightTT[] = { "g_mLightTT1", "g_mLightTT2", "g_mLightTT3" };

	if (m_isShadow && shadowMap)
	{
		m_ground.m_shader->SetTechnique("Scene_ShadowMap");

		for (int i = 0; i < shadowMapCount; ++i)
		{
			m_ground.m_shader->SetMatrix(varLightView[i], mLightView[i]);
			m_ground.m_shader->SetMatrix(varLightProj[i], mLightProj[i]);
			m_ground.m_shader->SetMatrix(varLightTT[i], mLightTT[i]);
			shadowMap[i].Bind(*m_ground.m_shader, varShadowMap[i]);
		}
	}
	else
	{
		m_ground.m_shader->SetTechnique("Scene_NoShadow");
	}

	m_ground.RenderShader(renderer, m_tm * tm);

	for (auto &shader : m_shaders)
	{
		if (m_isShadow && shadowMap)
		{
			shader->SetTechnique("Scene_ShadowMap");
			for (int i = 0; i < shadowMapCount; ++i)
			{
				shader->SetMatrix(varLightView[i], mLightView[i]);
				shader->SetMatrix(varLightProj[i], mLightProj[i]);
				shader->SetMatrix(varLightTT[i], mLightTT[i]);
				shadowMap[i].Bind(*shader, varShadowMap[i]);
			}
		}
		else
		{
			shader->SetTechnique("Scene_NoShadow");
		}
	}

	for (auto &p : m_models)
		if (p->m_isShow)
			p->RenderShader(renderer, tm);

	if (m_isDbgRender)
		m_dbgTile.Render(renderer, tm);
}


// return frustum cent to tile length
float cTile::CullingTest(const cFrustum &frustum
	, const bool isModel //= true
)
{
	if (frustum.IsInBox(m_boundingBox))
	{
		m_isShow = true;

		if (isModel)
		{
			for (auto &p : m_models)
			{
				p->m_isShow = frustum.IsInSphere(p->m_boundingSphere);
				if (p->m_isShow)
				{
					const Vector3 pos = p->m_boundingBox.Center() * p->m_tm;
					p->m_isShadow = pos.LengthRoughly(frustum.m_pos) < 100000;
				}
			}
		}
		else
		{
			for (auto &p : m_models)
				p->m_isShow = true;
		}
	}
	else
	{
		m_isShow = false;
	}

	return frustum.m_pos.LengthRoughly(m_boundingBox.Center());
}


bool cTile::AddModel(cModel2 *model)
{
	auto it = std::find(m_models.begin(), m_models.end(), model);
	if (m_models.end() != it)
		return false; // Already Exist

	m_models.push_back(model);

	if (model->m_shader)
		m_shaders.insert(model->m_shader);

	return true;
}


cModel2* cTile::FindModel(const int modelId)
{
	for (auto &p : m_models)
		if (modelId == p->m_id)
			return p;
	return NULL;
}


bool cTile::RemoveModel(cModel2 *model)
{
	auto it = std::find(m_models.begin(), m_models.end(), model);
	if (m_models.end() == it)
		return false; // Not Exist

	common::popvector2(m_models, model);
	m_shaders.erase(model->m_shader);
	return true;
}


void cTile::LostDevice()
{
}


void cTile::ResetDevice(cRenderer &renderer)
{
}


void cTile::Clear()
{
	for (auto &p : m_models)
		delete p;
	m_models.clear();
}
