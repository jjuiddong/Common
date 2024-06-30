#include "stdafx.h"
#include "sphere.h"


using namespace graphic;


cSphere::cSphere() 
	: cNode(common::GenerateId(), "sphere", eNodeType::MODEL)
	, m_radius(0)
	, m_texture(NULL)
{
	m_mtrl.InitWhite();
}

cSphere::cSphere(cRenderer &renderer, const float radius, const int stacks, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL)
	, const cColor &color //= cColor::WHITE
)
	: cNode(common::GenerateId(), "sphere", eNodeType::MODEL)
	, m_radius(0)
	, m_texture(NULL)
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
	m_vtxType = vtxType;
	m_transform.scale = Vector3::Ones * radius;

	const Transform tfm(Vector3(0, 0, 0), Vector3::Ones);
	m_boundingBox.SetBoundingBox(tfm);

	m_shape.Create(renderer, 1.f, stacks, slices, vtxType, color);
}


bool cSphere::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique(m_techniqueName.c_str());
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_transform.GetMatrixXM() * parentTm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);

	renderer.m_cbMaterial = m_mtrl.GetMaterial();
	renderer.m_cbMaterial.Update(renderer, 2);

	if ((m_shape.m_vtxType & eVertexType::TEXTURE0) && m_texture)
		m_texture->Bind(renderer, 0);

	if (IsRenderFlag(eRenderFlag::ALPHABLEND))
	{
		renderer.GetDevContext()->OMSetBlendState(renderer.m_renderState.NonPremultiplied(), 0, 0xffffffff);
		m_shape.Render(renderer);
		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	}
	else
	{
		renderer.GetDevContext()->OMSetBlendState(renderer.m_renderState.NonPremultiplied(), NULL, 0xffffffff);
		m_shape.Render(renderer);
		renderer.GetDevContext()->OMSetBlendState(renderer.m_renderState.Opaque(), NULL, 0xffffffff);
	}

	__super::Render(renderer, parentTm, flags);
	return true;
}


void cSphere::SetPos(const Vector3& pos)
{
	m_transform.pos = pos;
}


void cSphere::SetColor(const cColor &color)
{
	m_mtrl.m_diffuse = color.GetColor();
}


void cSphere::SetRadius(const float radius)
{
	m_radius = radius;
	m_transform.scale = Vector3::Ones * radius;
}
