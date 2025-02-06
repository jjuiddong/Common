
#include "stdafx.h"
#include "dbgaxis.h"

using namespace graphic;


cDbgAxis::cDbgAxis()
	: m_isAlphaBlending(true)
{
}

cDbgAxis::~cDbgAxis()
{
}


// create debug axis
// alpha: alph blending value
// length: line length
// width: line width
bool cDbgAxis::Create(cRenderer &renderer
	, const float alpha //= 0.5f
	, const float length // = 1.f
	, const float width // = 1.f
)
{
	const cColor colors[] = 
	{
		cColor(1.f, 0.f, 0.f, alpha)
		, cColor(0.f, 1.f, 0.f, alpha)
		, cColor(0.f, 0.f, 1.f, alpha)
	};
	m_lines[0].Create(renderer, Vector3(0, 0, 0), Vector3(length, 0, 0), width, colors[0]);
	m_lines[1].Create(renderer, Vector3(0, 0, 0), Vector3(0, length, 0), width, colors[1]);
	m_lines[2].Create(renderer, Vector3(0, 0, 0), Vector3(0, 0, length), width, colors[2]);

	m_lines[0].m_isSolid = true;
	m_lines[1].m_isSolid = true;
	m_lines[2].m_isSolid = true;

	return true;
}


void cDbgAxis::SetAxis(const cBoundingBox &bbox
	, const bool approximate //= true
)
{
	const Vector3 dim = bbox.GetDimension();
	const float size = max(dim.x, max(dim.y, dim.z)) * 0.005f;
	SetAxis(max(0.1f, min(1.f,size)), bbox, approximate);
}


void cDbgAxis::SetAxis(const float size, const cBoundingBox &bbox
	, const bool approximate //= true
)
{
	const Vector3 axis1[] = { Vector3(0.6f,0,0), Vector3(0,1.2f,0), Vector3(0,0,0.6f) };
	const Vector3 axis2[] = { Vector3(1,0,0), Vector3(0,1,0), Vector3(0,0,1) };

	const Matrix44 tm = bbox.GetMatrix();
	const Vector3 orig = Vector3(0, 0, 0) * tm;
	for (int i = 0; i < 3; ++i)
	{
		Vector3 v = ((approximate)? axis1[i] : axis2[i]) * tm;
		m_lines[i].SetLine(orig, v, v.IsEmpty()? 0.f : size);
	}
}


void cDbgAxis::Render(cRenderer &renderer
	, const XMMATRIX &tm //=XMIdentity
)
{
	for (int i = 0; i < 3; ++i)
		m_lines[i].Render(renderer, tm);
}
