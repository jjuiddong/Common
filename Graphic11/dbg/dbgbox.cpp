
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
	m_shape.Create(renderer, eVertexType::POSITION | eVertexType::DIFFUSE, color);
	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion(0,0,0,1));
}


void cDbgBox::Create(cRenderer &renderer, const cBoundingBox &bbox
	, const cColor &color //= cColor::BLACK
)
{
	m_shape.Create(renderer, eVertexType::POSITION | eVertexType::DIFFUSE, color);
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


void cDbgBox::SetBox(cRenderer &renderer, Vector3 vertices[8]
	, const cColor &color //= cColor::BLACK
)
{
	m_shape.Create(renderer, vertices, eVertexType::POSITION | eVertexType::DIFFUSE, color);
	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion(0, 0, 0, 1));
}


void cDbgBox::SetColor(DWORD color)
{
	assert(0);
}


// Render Box using Triangle
void cDbgBox::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->RSSetState(states.Wireframe());
	m_shape.Render(renderer);
	renderer.GetDevContext()->RSSetState(NULL);
}


XMMATRIX cDbgBox::GetTransform() const
{
	return m_boundingBox.GetTransform();
}
