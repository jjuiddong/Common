#include "stdafx.h"
#include "capsule.h"


using namespace graphic;


cCapsule::cCapsule()
	: cNode(common::GenerateId(), "capsule", eNodeType::MODEL)
	, m_radius(0)
	, m_halfHeight(0)
{
	m_mtrl.InitWhite();
}

cCapsule::cCapsule(cRenderer &renderer, const float radius, const float halfHeight
	, const int stacks, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= cColor::WHITE
)
	: cNode(common::GenerateId(), "cylinder", eNodeType::MODEL)
	, m_radius(0)
	, m_halfHeight(0)
{
	Create(renderer, radius, halfHeight, stacks, slices, vtxType, color);
}

cCapsule::~cCapsule()
{
}


bool cCapsule::Create(cRenderer &renderer, const float radius, const float halfHeight
	, const int stacks, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL)
	, const cColor &color //= cColor::WHITE
)
{
	m_mtrl.m_diffuse = color.GetColor();
	m_radius = radius;
	m_halfHeight = halfHeight;
	m_vtxType = vtxType;

	Transform tfm;
	tfm.scale = Vector3(1,1,1);
	m_boundingBox.SetBoundingBox(tfm);

	m_transform.scale = Vector3(halfHeight + radius, radius, radius);

	m_sphereShape.Create(renderer, 1.f, stacks, slices, vtxType, color);
	m_cylinderShape.Create(renderer, 1.f, 1.f, stacks, slices, vtxType, color);
	return true;
}


bool cCapsule::Render(cRenderer &renderer
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

	// render sphere first
	Transform tfm0;
	tfm0.pos = m_transform.pos;
	tfm0.pos += Vector3(m_halfHeight, 0, 0) * m_transform.rot;
	tfm0.scale = Vector3::Ones * m_transform.scale.y;

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(tfm0.GetMatrixXM() * parentTm);
	renderer.m_cbPerFrame.Update(renderer);
	m_sphereShape.Render(renderer);

	// render sphere second
	Transform tfm1;
	tfm1.pos = m_transform.pos;
	tfm1.pos += Vector3(-m_halfHeight, 0, 0) * m_transform.rot;
	tfm1.scale = Vector3::Ones * m_transform.scale.y;
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(tfm1.GetMatrixXM() * parentTm);
	renderer.m_cbPerFrame.Update(renderer);
	m_sphereShape.Render(renderer);

	// render cylinder
	Transform tfm2;
	tfm2.pos = m_transform.pos;
	tfm2.scale = Vector3(m_halfHeight, m_transform.scale.y, m_transform.scale.y);
	tfm2.rot = m_transform.rot;
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(tfm2.GetMatrixXM() * parentTm);
	renderer.m_cbPerFrame.Update(renderer);
	m_cylinderShape.Render(renderer);

	if (IsRenderFlag(eRenderFlag::ALPHABLEND))
		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	else
		renderer.GetDevContext()->OMSetBlendState(renderer.m_renderState.Opaque(), NULL, 0xffffffff);

	__super::Render(renderer, parentTm, flags);
	return true;
}


void cCapsule::SetColor(const cColor &color)
{
	m_mtrl.m_diffuse = color.GetColor();
}


void cCapsule::SetPos(const Vector3 &pos)
{
	m_transform.pos = pos;
}


void cCapsule::SetDimension(const float radius, const float halfHeight)
{
	m_radius = radius;
	m_halfHeight = halfHeight;
	m_transform.scale = Vector3(halfHeight + radius, radius, radius);
}
