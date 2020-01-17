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
	: cNode(common::GenerateId(), "sphere", eNodeType::MODEL)
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
	return m_shape.Create(renderer, radius, halfHeight, stacks, slices, vtxType, color);
}


bool cCapsule::Render(cRenderer &renderer
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


void cCapsule::SetPos(const Vector3 &pos)
{
	m_transform.pos = pos;
}
