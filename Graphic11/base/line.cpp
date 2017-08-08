
#include "stdafx.h"
#include "line.h"

using namespace graphic;


cLine::cLine() 
	: cNode2(common::GenerateId(), "line", eNodeType::MODEL)
{
}

cLine::cLine(cRenderer &renderer
	, const Vector3 &p0 //= Vector3(0, 0, 0)
	, const Vector3 &p1 //= Vector3(1, 1, 1)
	, const float width //= 1.f
	, const cColor color //= cColor::BLACK
)
	: cNode2(common::GenerateId(), "line", eNodeType::MODEL)
{
	m_shape.Create(renderer, (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE), color);
	SetLine(p0, p1, width);
}


bool cLine::Create(cRenderer &renderer
	, const Vector3 &p0 //= Vector3(0, 0, 0)
	, const Vector3 &p1 //= Vector3(1, 1, 1)
	, const float width //= 1.f
	, const cColor color //= cColor::BLACK
)
{
	m_shape.Create(renderer, (eVertexType::POSITION | eVertexType::NORMAL | eVertexType::DIFFUSE), color);
	SetLine(p0, p1, width);
	return true;
}


bool cLine::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_boundingBox.GetTransform());
	renderer.m_cbPerFrame.Update(renderer);

	m_shape.Render(renderer);
	return true;
}


void cLine::SetLine(const Vector3 &p0, const Vector3 &p1, const float width)
{
	m_boundingBox.SetLineBoundingBox(p0, p1, width);
}


void cLine::SetColor(const cColor &color)
{
	assert(0);
}

