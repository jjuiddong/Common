
#include "stdafx.h"
#include "dbgsphereline.h"

using namespace graphic;


cDbgSphereLine::cDbgSphereLine()
{
}

cDbgSphereLine::cDbgSphereLine(cRenderer &renderer
	, const float radius
	, const int slice
	, const cColor& color //= cColor::BLACK
)
	: cDbgCircleLine(renderer, radius, slice, color)
{
}


// Render sphere line using billboard circleline
void cDbgSphereLine::Render(cRenderer &renderer
	, const XMMATRIX &tm //= XMIdentity
)
{
	// billboard transform
	const XMMATRIX wtm = m_transform.GetMatrixXM() * tm; // world space
	Vector3 wpos; // world space pos
	XMStoreFloat3((XMFLOAT3*)&wpos, wtm.r[3]);
	const Vector3 dir = (GetMainCamera().GetEyePos() - wpos).Normal();
	Quaternion rot;
	rot.SetRotationArc(Vector3(0, 1, 0), dir);

	Transform tfm = m_transform;
	tfm.rot = rot;
	tfm.pos = wpos;
	tfm.scale = Vector3(m_radius, 1.f, m_radius);
	m_lines.Render(renderer, tfm.GetMatrixXM());
}


void cDbgSphereLine::RenderInstancing(cRenderer &renderer
	, const int count
	, const XMMATRIX *transforms
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	// todo: render instancing
}

