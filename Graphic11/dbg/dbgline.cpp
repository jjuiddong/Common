
#include "stdafx.h"
#include "dbgline.h"

using namespace graphic;


cDbgLine::cDbgLine()
{
}

cDbgLine::cDbgLine(cRenderer &renderer
	, const Vector3 &p0 //= Vector3(0, 0, 0)
	, const Vector3 &p1 //= Vector3(1, 1, 1)
	, const float width //= 1.f
	, const cColor color //= cColor::BLACK
)
	//: cLine(renderer, p0, p1, width, color)
{
	m_shape.Create(renderer, (eVertexType::POSITION | eVertexType::DIFFUSE), color);
	SetLine(p0, p1, width);
}


bool cDbgLine::Create(cRenderer &renderer
	, const Vector3 &p0 //= Vector3(0, 0, 0)
	, const Vector3 &p1 //= Vector3(1, 1, 1)
	, const float width //= 1.f
	, const cColor color //= cColor::BLACK
)
{
	m_shape.Create(renderer, (eVertexType::POSITION | eVertexType::DIFFUSE), color);
	SetLine(p0, p1, width);
	return true;
}


bool cDbgLine::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(XMIdentity);
	renderer.m_cbPerFrame.Update(renderer);

	CommonStates states(renderer.GetDevice());
	renderer.GetDevContext()->RSSetState(states.Wireframe());
	__super::Render(renderer, parentTm, flags);
	renderer.GetDevContext()->RSSetState(NULL);
	return true;
}

