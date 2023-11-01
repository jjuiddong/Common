
#include "stdafx.h"
#include "dbgcylinderline.h"

using namespace graphic;


cDbgCylinderLine::cDbgCylinderLine()
	: m_radius(1.f)
	, m_height(1.f)
{
}

cDbgCylinderLine::cDbgCylinderLine(cRenderer &renderer
	, const float radius
	, const float height
	, const int slice
	, const cColor& color //= cColor::BLACK
)
{
	Create(renderer, radius, height, slice, color);
}


bool cDbgCylinderLine::Create(cRenderer &renderer
	, const float radius
	, const float height
	, const int slice
	, const cColor& color //= cColor::BLACK
)
{
	if (slice < 3)
		return false;

	m_radius = radius;
	m_height = height;
	m_lines.Create(renderer, 4, color);
	m_lines.AddLine(renderer, Vector3(-1, 0.5f, 0), Vector3(-1, -0.5f, 0), false);
	m_lines.AddLine(renderer, Vector3(1, 0.5f, 0), Vector3(1, -0.5f, 0), false);
	m_lines.UpdateBuffer(renderer);
	m_circle.Create(renderer, radius, slice, color);
	return true;
}


// Render circle line
void cDbgCylinderLine::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	// render top circle
	{
		Transform tfm0;
		tfm0.pos.y = m_height * 0.5f;

		Transform tfm1;
		tfm1.pos = m_transform.pos;
		tfm1.rot = m_transform.rot;
		m_circle.Render(renderer, tfm0.GetMatrixXM() * tfm1.GetMatrixXM() * tm);
	}
	// render bottom circle
	{
		Transform tfm0;
		tfm0.pos.y = -m_height * 0.5f;

		Transform tfm1;
		tfm1.pos = m_transform.pos;
		tfm1.rot = m_transform.rot;
		m_circle.Render(renderer, tfm0.GetMatrixXM() * tfm1.GetMatrixXM() * tm);
	}

	// render cylinder side line
	{
		// billboard transform
		const Matrix44 wtm = m_transform.GetMatrixXM() * tm; // world tm
		const Matrix44 invTm = wtm.Inverse();
		const Vector3 camLocalPos = GetMainCamera().GetEyePos() * invTm;

		const Plane xzPlane(Vector3(0, 1, 0), 0);
		const Vector3 camPos = xzPlane.Projection(camLocalPos);
		Quaternion rot;
		rot.SetRotationArc(Vector3(0, 0, 1), camPos.Normal());

		Transform tfm = m_transform;
		tfm.scale = Vector3(m_radius, m_height, m_radius);
		tfm.rot = rot * m_transform.rot;
		m_lines.Render(renderer, tfm.GetMatrixXM() * tm);
	}
}


void cDbgCylinderLine::RenderInstancing(cRenderer &renderer
	, const int count
	, const XMMATRIX *transforms
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	// todo: render instancing
}


void cDbgCylinderLine::SetColor(const cColor& color)
{
	m_lines.m_color = color;
	m_circle.SetColor(color);
}


void cDbgCylinderLine::SetPos(const Vector3& pos)
{
	m_transform.pos = pos;
}


void cDbgCylinderLine::SetCylinder(const float radius, const float height)
{
	m_radius = radius;
	m_height = height;
}


void cDbgCylinderLine::SetRotation(const Quaternion& rot)
{
	m_transform.rot = rot;
}
