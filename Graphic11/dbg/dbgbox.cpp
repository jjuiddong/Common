
#include "stdafx.h"
#include "dbgbox.h"

using namespace graphic;


cDbgBox::cDbgBox()
{
}

cDbgBox::cDbgBox(cRenderer &renderer, const cBoundingBox &bbox
	, const cColor &color //= cColor::BLACK
)
{
	Create(renderer, bbox, color);
}


void cDbgBox::Create(cRenderer &renderer
	, const cColor &color //= cColor::BLACK
)
{
	m_color = color;
	m_shape.Create2(renderer, eVertexType::POSITION);
	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion(0,0,0,1));
}


void cDbgBox::Create(cRenderer &renderer, const cBoundingBox &bbox
	, const cColor &color //= cColor::BLACK
)
{
	m_color = color;
	m_shape.Create2(renderer, eVertexType::POSITION);
	SetBox(bbox);
}


void cDbgBox::SetBox(const cBoundingBox &bbox)
{
	m_boundingBox = bbox;
}


void cDbgBox::SetBox(const Transform &tfm)
{
	m_boundingBox.SetBoundingBox(tfm);
}

//
//void cDbgBox::SetBox(cRenderer &renderer, const Vector3 vertices[8]
//	, const cColor &color //= cColor::BLACK
//)
//{
//	m_shape.Create(renderer, vertices, eVertexType::POSITION | eVertexType::COLOR, color);
//	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion(0, 0, 0, 1));
//}


void cDbgBox::SetBox(cRenderer &renderer, const Vector3 vertices[8]
	, const cColor &color // = cColor::BLACK
)
{
	m_shape.SetCube(renderer, vertices, color);
	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion(0, 0, 0, 1));
}


void cDbgBox::SetColor(const cColor &color)
{
	m_color = color;
}


// Render Box using Triangle
void cDbgBox::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	cShader11 *shader = renderer.m_shaderMgr.FindShader(eVertexType::POSITION);
	assert(shader);
	shader->SetTechnique("Unlit");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_boundingBox.GetMatrixXM() * tm);
	renderer.m_cbPerFrame.Update(renderer);

	const Vector4 color = m_color.GetColor();
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(color.x, color.y, color.z, color.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	CommonStates states(renderer.GetDevice());
	ID3D11RasterizerState *oldState = NULL;
	renderer.GetDevContext()->RSGetState(&oldState);
	renderer.GetDevContext()->RSSetState(states.Wireframe());
	m_shape.Render(renderer);
	renderer.GetDevContext()->RSSetState(oldState);

	// debugging
	++renderer.m_drawCallCount;
#ifdef _DEBUG
	++renderer.m_shadersDrawCall[eVertexType::POSITION];
#endif
}


XMMATRIX cDbgBox::GetTransform() const
{
	return m_boundingBox.GetMatrixXM();
}
