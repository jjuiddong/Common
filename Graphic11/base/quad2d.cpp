
#include "stdafx.h"
#include "quad2d.h"


using namespace graphic;

cQuad2D::cQuad2D()
	: cNode(common::GenerateId(), "Quad2d", eNodeType::MODEL)
	, m_texture(NULL)
{
}

cQuad2D::~cQuad2D()
{
}


// 쿼드를 초기화 한다.
// width, height : 쿼드 크기
// pos : 쿼드 위치
bool cQuad2D::Create(cRenderer &renderer, const float x, const float y
	, const float width, const float height
	, const char *textureFileName // = " "
	, const Vector3 *quadVertices //= NULL
	, const Vector2 *quadUVs //= NULL
)
{
	const int vtxType = eVertexType::POSITION_RHW | eVertexType::COLOR | eVertexType::TEXTURE0;
	m_shape.Create(renderer, vtxType, cColor::WHITE, 2.f, 2.f, false, quadVertices, quadUVs);

	SetPosition(x, y, width, height);

	m_texture = cResourceManager::Get()->LoadTexture(renderer, textureFileName? textureFileName : g_defaultTexture);

	m_vtxType = vtxType;

	return true;
}


bool cQuad2D::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique("Unlit");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	UINT numVp = 1;
	D3D11_VIEWPORT vp;
	renderer.GetDevContext()->RSGetViewports(&numVp, &vp);

	// View * Projection 행렬을 mWorld에 저장한다.
	// 2D 모드에서는 View 행렬이 적용되지 않기 때문에, 
	// 바로 Projection 좌표계로 변환한다.
	// Shader에서는 View * Projection을 적용하지 않는다.

	const float halfWidth = m_transform.scale.x*2 / vp.Width;
	const float halfHeight = m_transform.scale.y*2 / vp.Height;

	Transform tfm = m_transform;
	tfm.pos.x = -1 + halfWidth + (m_transform.pos.x*2 / vp.Width);
	tfm.pos.y = 1 - halfHeight - (m_transform.pos.y * 2 / vp.Height);
	tfm.scale *= Vector3(2.f/ vp.Width, 2.f/ vp.Height, 1);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(tfm.GetMatrixXM());
	renderer.m_cbPerFrame.Update(renderer);

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);
	renderer.GetDevContext()->OMSetDepthStencilState(states.DepthNone(), 0);

	if (m_texture)
		m_texture->Bind(renderer, 0);

	m_shape.Render(renderer);

	renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	renderer.GetDevContext()->OMSetDepthStencilState(states.DepthDefault(), 0);
	return true;
}


void cQuad2D::SetPosition(const float x, const float y
	, const float width, const float height
)
{
	m_transform.pos = Vector3(x, y, 0);
	m_transform.scale = Vector3(width / 2, height / 2, 1);
}


cNode* cQuad2D::Picking(const Ray &ray, const eNodeType::Enum type
	, const XMMATRIX &parentTm
	, const bool isSpherePicking //= true
	, OUT float *dist //= NULL
)
{
	if (!(m_opFlags & eOpFlag::COLLISION))
		return NULL;

	// 2D 충돌 체크
	if (type == m_type)
	{
		const Vector2 pos = GetMainCamera().GetScreenPos(ray.orig);
		sRectf rect(m_transform.pos.x, m_transform.pos.y
			, m_transform.pos.x + m_transform.scale.x*2.f
			, m_transform.pos.y + m_transform.scale.y*2.f);
		if (rect.IsIn(pos.x, pos.y))
		{
			return this;
		}
	}

	if (m_children.empty())
		return NULL;

	vector<cNode*> picks;
	picks.reserve(4);

	for (auto &p : m_children)
		if (cNode *n = p->Picking(ray, type))
			picks.push_back(n);

	if (picks.empty())
		return NULL;

	if (picks.size() == 1)
		return picks[0];

	cNode *mostNearest = NULL;
	float nearLen = FLT_MAX;
	for (auto &p : picks)
	{
		const Vector3 modelPos = p->GetWorldMatrix().GetPosition();
		Plane ground(Vector3(0, 1, 0), modelPos);
		const Vector3 pickPos = ground.Pick(ray.orig, ray.dir);
		const float len = modelPos.LengthRoughly(pickPos);
		if (nearLen > len)
		{
			nearLen = len;
			mostNearest = p;
		}
	}

	return mostNearest;
}
