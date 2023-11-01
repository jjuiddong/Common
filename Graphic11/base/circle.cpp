
#include "stdafx.h"
#include "circle.h"

using namespace graphic;

cCircle::cCircle()
	: cNode(common::GenerateId(), "circle", eNodeType::MODEL)
	, m_radius(1)
	, m_color(cColor::WHITE)
{
}

cCircle::~cCircle()
{	
}


// 원 생성
bool cCircle::Create(cRenderer &renderer, const Vector3 &center
	, const float radius, const int slice
	, const cColor &color //= cColor::BLACK
	, const cCircleShape::ePlaneType planeType //= cCircleShape::ePlaneType::XY
)
{
	m_radius = radius;
	m_shape.Create(renderer, center, radius, slice, eVertexType::POSITION, color, planeType);
	m_vtxType = m_shape.m_vtxType;
	m_color = color;
	return true;
}


// 출력.
bool cCircle::Render(cRenderer &renderer
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

	const Vector4 diffuse = m_color.GetColor();
	renderer.m_cbMaterial.m_v->diffuse = diffuse.GetVectorXM();
	renderer.m_cbMaterial.Update(renderer, 2);

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
		renderer.GetDevContext()->OMSetBlendState(NULL, 0, 0xffffffff);
		m_shape.Render(renderer);
	}

	__super::Render(renderer, parentTm, flags);
	return true;
}


void cCircle::SetPos(const Vector3 &pos)
{
	m_transform.pos = pos;
}


void cCircle::SetColor(const cColor& color)
{
	m_color = color;
}
