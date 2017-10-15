
#include "stdafx.h"
#include "torus.h"

using namespace graphic;


cTorus::cTorus()
	: m_color(cColor::WHITE)
{
}

cTorus::~cTorus()
{
}


bool cTorus::Create(cRenderer &renderer, const float outerRadius, const float innerRadius
	, const int stack //= 10
	, const int slice //= 10
	, const cColor &color //= cColor::WHITE
)
{
	m_shape.Create(renderer, outerRadius, innerRadius, stack, slice);
	m_color = color;
	return true;
}


bool cTorus::Render(cRenderer &renderer
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

	const Vector4 color = m_color.GetColor();
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(color.x, color.y, color.z, color.w);
	renderer.m_cbMaterial.Update(renderer, 2);


	m_shape.Render(renderer);
	return true;
}
