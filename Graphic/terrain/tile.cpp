#include "stdafx.h"
#include "tile.h"

using namespace graphic;


cTile::cTile()
	: cNode2(common::GenerateId(), "tile", eNodeType::TERRAIN)
	, m_isDbgRender(false)
	, m_location(-1,-1)
{
}

cTile::~cTile()
{
	Clear();
}


bool cTile::Create(cRenderer &renderer
	, const int id
	, const Str64 &name
	, const sRectf &rect
	, const float y //=0
	, const float uvFactor //= 1.f,
	, const Vector2 &uv0 //= Vector2(0, 0)
	, const Vector2 &uv1 //= Vector2(1, 1)
)
{
	m_id = id;
	m_name = name;

	const float cellSize = rect.Width() / 2.f;
	m_ground.Create(renderer, 2, 2, cellSize, 1, y, uv0, uv1);
	m_ground.SetShader(cResourceManager::Get()->LoadShader(renderer, "tile.fx"));
	m_ground.m_shader->SetFloat("g_uvFactor", uvFactor);
	m_ground.m_mtrl.InitXFile();
	
	m_transform.pos = Vector3(rect.left + cellSize, 0, rect.top + cellSize);
	const Matrix44 tm = m_transform.GetMatrix();

	m_boundingBox.SetBoundingBox(Vector3(-cellSize, 0, -cellSize)
		, Vector3(cellSize, 20, cellSize));

	m_dbgTile.SetBox(renderer, Vector3(-cellSize, 0, -cellSize)
		, Vector3(cellSize, 20, cellSize));

	CalcBoundingSphere();

	return true;
}


bool cTile::Create(cRenderer &renderer
	, const int id
	, const Str64 &name
	, const Vector3 &dim
	, const Transform &transform
	, const float y //= 0
	, const float uvFactor //= 1.f
	, const Vector2 &uv0 //= Vector2(0, 0)
	, const Vector2 &uv1 //= Vector2(1, 1)
)
{
	m_id = id;
	m_name = name;

	const float cellSize = dim.x / 2.f;
	m_ground.Create(renderer, 2, 2, cellSize, 1, y, uv0, uv1);
	m_ground.SetShader(cResourceManager::Get()->LoadShader(renderer, "tile.fx"));
	m_ground.m_shader->SetFloat("g_uvFactor", uvFactor);
	m_ground.m_mtrl.InitXFile();

	m_transform = transform;
	const Matrix44 tm = m_transform.GetMatrix();

	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), dim);
	m_dbgTile.SetBox(renderer, Vector3(0, 0, 0), dim);

	CalcBoundingSphere();
	
	return true;
}


void cTile::UpdateShader(const Matrix44 *mLightView, const Matrix44 *mLightProj, const Matrix44 *mLightTT
	, cShadowMap *shadowMap //= NULL
	, const int shadowMapCount //= 0
)
{
	RET(!m_isEnable);
	RET(!m_isShow);

	char *varShadowMap[] = { "g_shadowMapTexture1", "g_shadowMapTexture2", "g_shadowMapTexture3" };
	char *varLightView[] = { "g_mLightView1", "g_mLightView2", "g_mLightView3" };
	char *varLightProj[] = { "g_mLightProj1", "g_mLightProj2", "g_mLightProj3" };
	char *varLightTT[] = { "g_mLightTT1", "g_mLightTT2", "g_mLightTT3" };

	cCamera *cam = GetMainCamera();

	if (m_isShadow && shadowMap)
	{
		m_ground.m_shader->SetTechnique("Scene_ShadowMap");
		cam->Bind(*m_ground.m_shader);

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

	for (auto &shader : m_shaders)
	{
		cam->Bind(*shader);

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

	for (auto &p : m_children)
		if (eNodeType::TERRAIN == p->m_nodeType)
			((cTile*)p)->UpdateShader(mLightView, mLightProj, mLightTT, shadowMap, shadowMapCount);
}


bool cTile::Update(cRenderer &renderer, const float deltaSeconds)
{
	__super::Update(renderer, deltaSeconds);

	return true;
}


void cTile::PreRender(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	RET(!m_isShadow);
	RET(!m_isEnable);
	RET(!m_isShow);

	cCamera *cam = GetMainCamera();
	for (auto &shader : m_shaders)
	{
		shader->SetTechnique("ShadowMap");
		cam->Bind(*shader);
	}

	const Matrix44 transform = m_transform.GetMatrix() * tm;
	for (auto &p : m_children)
	{
		if (!p->m_isShadow)
			continue;
		p->Render(renderer, transform, 0x8 | 0x1); // shadow + visible
	}
}


bool cTile::Render(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
	, const int flags //= 1
)
{
	RETV(!m_isEnable, false);
	RETV(!m_isShow, false);

	const Matrix44 transform = m_transform.GetMatrix() * tm;
	m_ground.RenderShader(renderer, transform);

	if ((flags & 0x1) && m_isDbgRender)
		DebugRender(renderer, tm);

	__super::Render(renderer, tm, flags);

	return true;
}


void cTile::DebugRender(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	const Matrix44 transform = m_transform.GetMatrix() * tm;
	m_dbgTile.Render(renderer, transform);
}


// return frustum center to tile length
float cTile::CullingTest(const cFrustum &frustum
	, const Matrix44 &tm //= Matrix44::Identity
	, const bool isModel //= true
)
{
	RETV(!m_isEnable, false);

	const Matrix44 transform = m_transform.GetMatrix()*tm;

	if (frustum.IsInSphere(m_boundingSphere, transform))
	{
		m_isShow = true;
		m_isShadow = m_isShadowEnable;

		if (isModel)
		{
			for (auto &p : m_children)
				p->CullingTest(frustum, transform, isModel);
		}
		else
		{
			for (auto &p : m_children)
				p->m_isShow = true;
		}
	}
	else
	{
		m_isShow = false;
	}

	return frustum.m_pos.LengthRoughly(m_boundingBox.Center() * transform);
}


bool cTile::AddChild(cNode2 *node)
{
	if (__super::AddChild(node))
		if (node->m_shader)
			m_shaders.insert(node->m_shader);

	return true;
}


bool cTile::RemoveChild(cNode2 *rmNode
	, const bool rmInstance //= true
)
{
	__super::RemoveChild(rmNode, rmInstance);
	return true;
}


const cNode2* cTile::FindNode(const int id) const
{
	return __super::FindNode(id);
}


void cTile::LostDevice()
{
	for (auto &p : m_children)
		p->LostDevice();
}


void cTile::ResetDevice(cRenderer &renderer)
{
	for (auto &p : m_children)
		p->ResetDevice(renderer);
}


void cTile::Clear()
{
}
