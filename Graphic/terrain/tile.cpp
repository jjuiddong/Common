
#include "stdafx.h"
#include "tile.h"

using namespace graphic;


cTile::cTile()
	: m_isShadow(true)
	, m_isCulling(false)
	, m_isDbgRender(true)
	, m_dbgIdx(2)
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
	, const int dbgIdx //=2
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

	//m_shadowMap.Create(renderer, 800, 800);

	m_dbgTile.SetBox(renderer, Vector3(rect.left, 0, rect.top)
		, Vector3(rect.right, 20, rect.bottom));

	//const Vector3 p0 = lightPos;// Vector3(center.x, 20, center.y);
	//const Vector3 p1 = (lightLookat - lightPos).Normal() * 3 + p0;
	//m_dbgLight.Create(renderer, p0, p1, 0.5F);

	m_dbgIdx = dbgIdx;

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
	RET(m_isCulling);

	//m_VPT = view * proj * tt;
	//m_LVP = view * proj;
	cCamera *cam = GetMainCamera();

	if (!m_models.empty())
	{
		cShader *shader = m_models[0]->m_shader;
		shader->SetTechnique("ShadowMap");
		//shader->SetTechnique("Scene_NoShadow");
		shader->SetMatrix("g_mView", cam->GetViewMatrix());
		shader->SetMatrix("g_mProj", cam->GetProjectionMatrix());
	}

	for (auto &p : m_models)
	{
		if (!p->m_isShadow)
			continue;
  		p->RenderShader(renderer);
	}
}


void cTile::Render(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	RET(m_isCulling);

	m_ground.m_shader->SetTechnique("Scene_NoShadow");

	m_ground.RenderShader(renderer, m_tm * tm);

	if (!m_models.empty())
	{
		cShader *shader = m_models[0]->m_shader;
		shader->SetTechnique("Scene_NoShadow");
	}

	for (auto &p : m_models)
		p->RenderShader(renderer, tm);

	if (m_isDbgRender)
		m_dbgTile.Render(renderer, tm);
}


void cTile::Render( cRenderer &renderer, const Matrix44 &mVPT, const Matrix44 &mLVP
	, cShadowMap *shadowMap //= NULL
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	RET(m_isCulling);

	if (shadowMap)
	{
		m_ground.m_shader->SetMatrix("g_mVPT", mVPT);
		m_ground.m_shader->SetMatrix("g_mLVP", mLVP);
	}

	m_ground.m_shader->SetTechnique("Scene_ShadowMap");

	if (shadowMap)
		shadowMap->Bind(*m_ground.m_shader, "g_shadowMapTexture");

	m_ground.RenderShader(renderer, m_tm * tm);

	if (!m_models.empty())
	{
		cShader *shader = m_models[0]->m_shader;
		shader->SetTechnique("Scene_ShadowMap");
		shader->SetMatrix("g_mVPT", mVPT);
		shader->SetMatrix("g_mLVP", mLVP);
		if (shadowMap)
			shadowMap->Bind(*shader, "g_shadowMapTexture");
	}

	for (auto &p : m_models)
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
		m_isCulling = false;

		if (isModel)
		{
			for (auto &p : m_models)
			{
				p->m_isShow = frustum.IsIn(p->m_tm.GetPosition());
				if (p->m_isShow)
					p->m_isShadow = p->m_tm.GetPosition().LengthRoughly(frustum.m_pos) < 100000;
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
		m_isCulling = true;
	}

	return frustum.m_pos.LengthRoughly(m_boundingBox.Center());
}


bool cTile::AddModel(cModel2 *model)
{
	auto it = std::find(m_models.begin(), m_models.end(), model);
	if (m_models.end() != it)
		return false; // Already Exist

	m_models.push_back(model);
	return true;
}


bool cTile::RemoveModel(cModel2 *model)
{
	auto it = std::find(m_models.begin(), m_models.end(), model);
	if (m_models.end() == it)
		return false; // Not Exist

	common::popvector2(m_models, model);
	return true;
}


void cTile::LostDevice()
{
	//m_shadowMap.LostDevice();
}


void cTile::ResetDevice(cRenderer &renderer)
{
	//m_shadowMap.ResetDevice(renderer);
}


void cTile::Clear()
{
	for (auto &p : m_models)
		delete p;
	m_models.clear();
}
