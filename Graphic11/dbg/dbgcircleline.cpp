
#include "stdafx.h"
#include "dbgcircleline.h"

using namespace graphic;


cDbgCircleLine::cDbgCircleLine()
	: m_radius(1.f)
{
}

cDbgCircleLine::cDbgCircleLine(cRenderer &renderer
	, const float radius
	, const int slice
	, const cColor& color //= cColor::BLACK
)
{
	Create(renderer, radius, slice, color);
}


bool cDbgCircleLine::Create(cRenderer &renderer
	, const float radius, const int slice
	, const cColor& color //= cColor::BLACK
)
{
	if (slice < 3)
		return false;

	m_radius = radius;
	m_lines.Create(renderer, slice, color);
	
	// create circle line, X-Z plane
	const float angle = (MATH_PI * 2.f) / (float)slice;
	for (int i = 0; i < slice; ++i)
	{		
		const float x0 = cos(i * angle);
		const float z0 = sin(i * angle);
		const float x1 = cos((i + 1) * angle);
		const float z1 = sin((i + 1) * angle);
		m_lines.AddLine(renderer, Vector3(x0, 0, z0), Vector3(x1, 0, z1), false);
	}
	m_lines.UpdateBuffer(renderer);
	//~
	return true;
}


// Render circle line
void cDbgCircleLine::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	Transform tfm = m_transform;
	tfm.scale = Vector3(m_radius, 1.f, m_radius);
	m_lines.Render(renderer, tfm.GetMatrixXM() * tm);
}


void cDbgCircleLine::RenderInstancing(cRenderer &renderer
	, const int count
	, const XMMATRIX *transforms
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	// todo: render instancing
}


void cDbgCircleLine::SetColor(const cColor& color)
{
	m_lines.m_color = color;
}


void cDbgCircleLine::SetPos(const Vector3& pos)
{
	m_transform.pos = pos;
}


void cDbgCircleLine::SetRadius(const float radius)
{
	m_radius = radius;
}


void cDbgCircleLine::SetRotation(const Quaternion& rot)
{
	m_transform.rot = rot;
}
