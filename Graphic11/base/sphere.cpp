#include "stdafx.h"
#include "sphere.h"


using namespace graphic;


cSphere::cSphere() 
	: cNode2(common::GenerateId(), "sphere", eNodeType::MODEL)
	, m_radius(0)
{
	m_mtrl.InitWhite();
}

cSphere::cSphere(cRenderer &renderer, const float radius, const int stacks, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL)
	, const cColor &color //= cColor::WHITE
)
	: cNode2(common::GenerateId(), "sphere", eNodeType::MODEL)
	, m_radius(0)
{
	Create(renderer, radius, stacks, slices, vtxType, color);	
}

cSphere::~cSphere()
{
}


void cSphere::Create(cRenderer &renderer, const float radius, const int stacks, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL)
	, const cColor &color //= cColor::WHITE
)
{
	m_mtrl.m_diffuse = color.GetColor();
	m_radius = radius;
	m_shape.Create(renderer, radius, stacks, slices, vtxType);
}


bool cSphere::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique("Unlit");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_transform.GetMatrixXM() * parentTm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);

	renderer.m_cbMaterial = m_mtrl.GetMaterial();
	renderer.m_cbMaterial.Update(renderer, 2);

	m_shape.Render(renderer);

	__super::Render(renderer, parentTm, flags);
	return true;
}
