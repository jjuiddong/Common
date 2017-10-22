
#include "stdafx.h"
#include "circle.h"

using namespace graphic;

cCircle::cCircle()
	: cNode(common::GenerateId(), "circle", eNodeType::MODEL)
	, m_radius(1)
{
}

cCircle::~cCircle()
{	
}


// 원 생성
bool cCircle::Create(cRenderer &renderer, const Vector3 &center, const float radius, const int slice
	, const cColor &color //= cColor::BLACK
)
{
	m_radius = radius;
	m_shape.Create(renderer, center, radius, slice, color);
	return true;
}


// 출력.
void cCircle::Render(cRenderer &renderer)
{
	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique("Unlit");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_transform.GetMatrixXM());
	renderer.m_cbPerFrame.Update(renderer);

	m_shape.Render(renderer);
}
