
#include "stdafx.h"
#include "dbgsphere.h"


using namespace graphic;

cDbgSphere::cDbgSphere()
	: m_color(cColor::BLACK)
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
	m_color = color;
	m_shape.Create(renderer, radius, stacks, slices, eVertexType::POSITION, color);
}


void cDbgSphere::Render(cRenderer &renderer
	, const XMMATRIX &tm //=XMIdentity
)
{
	cShader11 *shader = renderer.m_shaderMgr.FindShader(eVertexType::POSITION);
	assert(shader);
	shader->SetTechnique("Unlit");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_transform.GetMatrixXM() * tm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);

	const Vector4 color = m_color.GetColor();
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(color.x, color.y, color.z, color.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->RSSetState(states.Wireframe());
	m_shape.Render(renderer);
	renderer.GetDevContext()->RSSetState(NULL);

	// debugging
	++renderer.m_drawCallCount;
#ifdef _DEBUG
	++renderer.m_shadersDrawCall[eVertexType::POSITION];
#endif
}


XMMATRIX cDbgSphere::GetTransform() const
{
	return m_transform.GetMatrixXM();
}


void cDbgSphere::SetPos(const Vector3 &pos) 
{
	m_transform.pos = pos;
}


void cDbgSphere::SetRadius(const float radius)
{
	m_transform.scale = Vector3(1, 1, 1)*radius;
}


void cDbgSphere::SetColor(const cColor &color)
{
	m_color = color;
}
