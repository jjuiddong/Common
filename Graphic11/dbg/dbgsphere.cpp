
#include "stdafx.h"
#include "dbgsphere.h"


using namespace graphic;

cDbgSphere::cDbgSphere()
{
}

cDbgSphere::cDbgSphere(cRenderer &renderer, const float radius, const int stacks, const int slices
	, const cColor &color //= cColor::BLACK
)
{
	Create(renderer, radius, stacks, slices, color);
}

cDbgSphere::~cDbgSphere()
{
}


// ±¸ »ý¼º
void cDbgSphere::Create(cRenderer &renderer, const float radius, const int stacks, const int slices
	, const cColor &color //= cColor::BLACK
)
{
	m_bsphere.SetBoundingSphere(Vector3(0, 0, 0), radius);
	m_shape.Create(renderer, radius, stacks, slices, eVertexType::POSITION | eVertexType::DIFFUSE, color);
}


void cDbgSphere::Render(cRenderer &renderer
	, const XMMATRIX &tm //=XMIdentity
)
{
	cShader11 *shader = renderer.m_shaderMgr.FindShader(eVertexType::POSITION | eVertexType::DIFFUSE);
	assert(shader);
	shader->SetTechnique("Unlit");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_bsphere.GetTransformXM() * tm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);
	renderer.m_cbMaterial.Update(renderer, 2);

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->RSSetState(states.Wireframe());
	m_shape.Render(renderer);
	renderer.GetDevContext()->RSSetState(NULL);
}


XMMATRIX cDbgSphere::GetTransform() const
{
	return m_bsphere.GetTransformXM();
}


void cDbgSphere::SetPos(const Vector3 &pos) 
{
	m_bsphere.SetPos(pos);
}
