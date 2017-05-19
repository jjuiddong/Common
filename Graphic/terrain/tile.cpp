
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
	
	Matrix44 T;
	T.SetPosition(Vector3(rect.left+cellSize, 0, rect.top + cellSize));
	m_tm = T;

	//m_light.Init(cLight::LIGHT_DIRECTIONAL,
	//	Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1),
	//	Vector4(0.2f, 0.2f, 0.2f, 1));
	//const Vector2 center = rect.Center();
	//const Vector3 lightLookat = Vector3(center.x, 0, center.y);
	//const Vector3 lightPos = Vector3(1, -1, 1).Normal() * -400.f + lightLookat;
	//m_light.SetPosition(lightPos);
	//m_light.SetDirection((lightLookat - lightPos).Normal());

	m_boundingBox.SetBoundingBox(Vector3(rect.left, 0, rect.top)
		, Vector3(rect.right, 20, rect.bottom));

	//m_shadowMap.Create(renderer, 800, 800);

	m_dbgTile.SetBox(renderer, Vector3(rect.left, 0, rect.top)
		, Vector3(rect.right, 20, rect.bottom));

	//const Vector3 p0 = lightPos;// Vector3(center.x, 20, center.y);
	//const Vector3 p1 = (lightLookat - lightPos).Normal() * 3 + p0;
	//m_dbgLight.Create(renderer, p0, p1, 0.5F);

	//Matrix44 view, proj, tt;
	//m_light.GetShadowMatrix(view, proj, tt);
	//m_dbgLightFrustum.Create(renderer, view * proj);

	m_dbgIdx = dbgIdx;

	return true;
}


void cTile::Update(cRenderer &renderer, const float deltaSeconds)
{
	for (auto &p : m_models)
		p->Update(renderer, deltaSeconds);
}


void cTile::PreRender(cRenderer &renderer, cShadowMap &shadowMap)
{
	RET(!m_isShadow);
	RET(m_isCulling);

	//Matrix44 view, proj, tt;
	//m_light.GetShadowMatrix(view, proj, tt);
	//m_VPT = view * proj * tt;
	//m_LVP = view * proj;

	if (!m_models.empty())
	{
		cShader *shader = m_models[0]->m_shader;
		shader->SetTechnique("ShadowMap");
		//shader->SetMatrix("g_mView", view);
		//shader->SetMatrix("g_mProj", proj);
	}

	shadowMap.Begin(renderer);
	for (auto &p : m_models)
	{
		//if (!p->m_isShadow)
		//	continue;
  		p->RenderShader(renderer);
	}
	shadowMap.End(renderer);
}


void cTile::Render(cRenderer &renderer
	, cShadowMap *shadowMap //= NULL
)
{
	RET(m_isCulling);

	if (shadowMap)
	{
		m_ground.m_shader->SetMatrix("g_mVPT", m_VPT);
		m_ground.m_shader->SetMatrix("g_mLVP", m_LVP);
	}

	m_ground.m_shader->SetTechnique((shadowMap)? "Scene_ShadowMap" : "Scene_NoShadow");

	if (shadowMap)
		shadowMap->Bind(*m_ground.m_shader, "g_shadowMapTexture");

	m_ground.RenderShader(renderer, m_tm);

	if (!m_models.empty())
	{
		cShader *shader = m_models[0]->m_shader;
		shader->SetTechnique((shadowMap) ? "Scene_ShadowMap" : "Scene_NoShadow");
		shader->SetMatrix("g_mVPT", m_VPT);
		shader->SetMatrix("g_mLVP", m_LVP);
		if (shadowMap)
			shadowMap->Bind(*shader, "g_shadowMapTexture");
	}

	for (auto &p : m_models)
		p->RenderShader(renderer);

	if (m_isDbgRender)
	{
		m_dbgTile.Render(renderer);
		//m_dbgLight.Render(renderer);
		//m_dbgLightFrustum.Render(renderer);
		//m_shadowMap.RenderShadowMap(renderer, m_dbgIdx);
	}
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
					p->m_isShadow = p->m_tm.GetPosition().LengthRoughly(frustum.m_pos) < 10000;
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
