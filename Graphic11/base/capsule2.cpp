#include "stdafx.h"
#include "capsule2.h"


using namespace graphic;


cCapsule2::cCapsule2()
	: cNode(common::GenerateId(), "capsule", eNodeType::MODEL)
	, m_radius(0)
	, m_halfHeight(0)
{
	m_mtrl.InitWhite();
}

cCapsule2::cCapsule2(cRenderer &renderer, const float radius, const float halfHeight
	, const int stacks, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= cColor::WHITE
)
	: cNode(common::GenerateId(), "capsule", eNodeType::MODEL)
	, m_radius(0)
	, m_halfHeight(0)
{
	Create(renderer, radius, halfHeight, stacks, slices, vtxType, color);
}

cCapsule2::~cCapsule2()
{
}


bool cCapsule2::Create(cRenderer &renderer, const float radius, const float halfHeight
	, const int stacks, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= cColor::WHITE
)
{
	m_mtrl.m_diffuse = color.GetColor();
	m_radius = radius;
	m_halfHeight = halfHeight;
	m_vtxType = vtxType;

	Transform tfm;
	tfm.scale = Vector3(radius, halfHeight + radius, radius);
	m_boundingBox.SetBoundingBox(tfm);

	m_shape.Create(renderer, 1.f, 1.f, stacks, slices, vtxType, color);
	return true;
}


bool cCapsule2::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_vtxType);
	assert(shader);
	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbMaterial = m_mtrl.GetMaterial();
	renderer.m_cbMaterial.Update(renderer, 2);

	if (IsRenderFlag(eRenderFlag::ALPHABLEND))
		renderer.GetDevContext()->OMSetBlendState(renderer.m_renderState.NonPremultiplied(), 0, 0xffffffff);
	else
		renderer.GetDevContext()->OMSetBlendState(renderer.m_renderState.NonPremultiplied(), NULL, 0xffffffff);

	m_shape.Render(renderer, m_radius, m_halfHeight, m_transform.GetMatrixXM() * parentTm);

	if (IsRenderFlag(eRenderFlag::ALPHABLEND))
		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	else
		renderer.GetDevContext()->OMSetBlendState(renderer.m_renderState.Opaque(), NULL, 0xffffffff);

	__super::Render(renderer, parentTm, flags);
	return true;
}


void cCapsule2::SetColor(const cColor &color)
{
	m_mtrl.m_diffuse = color.GetColor();
}


void cCapsule2::SetPos(const Vector3 &pos)
{
	m_transform.pos = pos;
}


void cCapsule2::SetDimension(const float radius, const float halfHeight)
{
	m_radius = radius;
	m_halfHeight = halfHeight;
}


void cCapsule2::SetCapsule(const Vector3& p0, const Vector3& p1, const float radius)
{
	const float len = p0.Distance(p1);
	const Vector3 dir = (p1 - p0).Normal();
	m_radius = radius;
	m_halfHeight = len / 2.f;
	m_transform.pos = (p0 + p1) * 0.5f;
	m_transform.rot.SetRotationArc(Vector3(0, 1, 0), dir);
}
