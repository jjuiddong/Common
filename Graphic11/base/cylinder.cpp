#include "stdafx.h"
#include "cylinder.h"

using namespace graphic;


cCylinder::cCylinder()
	: cNode(common::GenerateId(), "capsule", eNodeType::MODEL)
	, m_radius(0)
	, m_height(0)
{
	m_mtrl.InitWhite();
}

cCylinder::cCylinder(cRenderer &renderer, const float radius, const float height
	, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::COLOR)
	, const cColor &color //= cColor::WHITE
)
	: cNode(common::GenerateId(), "cylinder", eNodeType::MODEL)
	, m_radius(0)
	, m_height(0)
{
	Create(renderer, radius, height, slices, vtxType, color);
}

cCylinder::~cCylinder()
{
}


bool cCylinder::Create(cRenderer &renderer, const float radius, const float height
	, const int slices
	, const int vtxType //= (eVertexType::POSITION | eVertexType::NORMAL)
	, const cColor &color //= cColor::WHITE
)
{
	m_mtrl.m_diffuse = color.GetColor();
	m_radius = radius;
	m_height = height;
	m_vtxType = vtxType;

	Transform tfm;
	tfm.scale = Vector3(1, 1, 1);
	m_boundingBox.SetBoundingBox(tfm);

	m_transform.scale = Vector3(height, radius, radius);

	return m_shape.Create(renderer, 1.f, 1.f, slices, vtxType, color);
}


bool cCylinder::Render(cRenderer &renderer
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

	//if ((m_shape.m_vtxType & eVertexType::TEXTURE0) && m_texture)
	//	m_texture->Bind(renderer, 0);

	if (IsRenderFlag(eRenderFlag::ALPHABLEND))
	{
		CommonStates states(renderer.GetDevice());
		renderer.GetDevContext()->OMSetBlendState(states.NonPremultiplied(), 0, 0xffffffff);
		m_shape.Render(renderer);
		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
	}
	else
	{
		CommonStates state(renderer.GetDevice());
		renderer.GetDevContext()->OMSetBlendState(state.NonPremultiplied(), NULL, 0xffffffff);
		m_shape.Render(renderer);
		renderer.GetDevContext()->OMSetBlendState(state.Opaque(), NULL, 0xffffffff);
	}

	__super::Render(renderer, parentTm, flags);
	return true;
}


void cCylinder::SetColor(const cColor &color)
{
	m_mtrl.m_diffuse = color.GetColor();
}


void cCylinder::SetPos(const Vector3 &pos)
{
	m_transform.pos = pos;
}


void cCylinder::SetDimension(const float radius, const float height)
{
	m_radius = radius;
	m_height = height;
	m_transform.scale = Vector3(height, radius, radius);
}
