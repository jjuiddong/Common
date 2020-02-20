
#include "stdafx.h"
#include "camera3d.h"


using namespace graphic;

cCamera3D::cCamera3D(const char *name)
	: cCamera(name)
{
	UpdateViewMatrix();
}

cCamera3D::cCamera3D(const char *name, const Vector3 &eyePos, const Vector3 &lookAt, const Vector3 &up)
	: cCamera(name)
{
	m_eyePos = eyePos;
	m_lookAt = lookAt;
	m_up = up;
	UpdateViewMatrix();
}

cCamera3D::~cCamera3D()
{
}


void cCamera3D::Update(const float deltaSeconds)
{
	__super::Update(deltaSeconds);

	if (m_isKeepHorizontal)
		KeepHorizontal();

	UpdateViewMatrix();
}


void cCamera3D::Bind(cRenderer &renderer)
{
	if (m_isOrthogonal)
	{
		Matrix44 zoom;
		zoom.SetScale(Vector3(1, 1, 1));
		const XMMATRIX mView = XMLoadFloat4x4((XMFLOAT4X4*)&m_view);
		const XMMATRIX mProj = XMLoadFloat4x4((XMFLOAT4X4*)&m_proj);
		renderer.m_cbPerFrame.m_v->mView = XMMatrixTranspose(zoom.GetMatrixXM()*mView);
		renderer.m_cbPerFrame.m_v->mProjection = XMMatrixTranspose(mProj);
		renderer.m_cbPerFrame.m_v->eyePosW = m_eyePos.GetVectorXM();
	}
	else
	{
		const XMMATRIX mView = XMLoadFloat4x4((XMFLOAT4X4*)&m_view);
		const XMMATRIX mProj = XMLoadFloat4x4((XMFLOAT4X4*)&m_proj);
		renderer.m_cbPerFrame.m_v->mView = XMMatrixTranspose(mView);
		renderer.m_cbPerFrame.m_v->mProjection = XMMatrixTranspose(mProj);
		renderer.m_cbPerFrame.m_v->eyePosW = m_eyePos.GetVectorXM();
	}
}


void cCamera3D::UpdateViewMatrix()
{
	m_view.SetView2(m_eyePos, m_lookAt, m_up);
	m_viewProj = m_view * m_proj;
}


// ViewMatrix 로 eyePos, lookAt, up 을 계산한다.
void cCamera3D::UpdateParameterFromViewMatrix()
{
	const Vector3 right(m_view._11, m_view._21, m_view._31);
	const Vector3 up(m_view._12, m_view._22, m_view._32);
	const Vector3 dir(m_view._13, m_view._23, m_view._33);
	const Vector3 pos = m_view.Inverse().GetPosition();

	m_eyePos = pos;
	m_up = up;
	m_lookAt = dir * 100 + pos;
}


// Right 축으로 회전한다.
void cCamera3D::Pitch2(const float radian)
{
	RET(radian == 0);

	const Vector3 axis = GetRight();
	const Quaternion q(axis, radian);
	const Matrix44 mat = q.GetMatrix();

	Vector3 v = m_eyePos - m_lookAt;
	v *= mat;
	Vector3 eyePos = m_lookAt + v;

	Vector3 dir = m_lookAt - eyePos;
	dir.Normalize();

	// Vertical Direction BugFix
	if (abs(Vector3(0, 1, 0).DotProduct(dir)) > 0.98f)
		return;

	m_eyePos = eyePos;

	UpdateViewMatrix();
}


// Right 축으로 회전한다.
void cCamera3D::Pitch2(const float radian, const Vector3 &up)
{
	RET(radian == 0);

	const Vector3 right = up.CrossProduct(GetDirection()).Normal();
	const Quaternion q(right, radian);
	const Matrix44 mat = q.GetMatrix();

	Vector3 v = m_eyePos - m_lookAt;
	v *= mat;
	Vector3 eyePos = m_lookAt + v;

	Vector3 dir = m_lookAt - eyePos;
	dir.Normalize();

	// Vertical Direction BugFix
	if (abs(Vector3(0, 1, 0).DotProduct(dir)) > 0.98f)
		return;

	m_eyePos = eyePos;
	m_up = up;

	UpdateViewMatrix();
}


// Up 축으로 회전한다.
void cCamera3D::Yaw2(const float radian)
{
	RET(radian == 0);

	const Vector3 axis = GetUpVector();
	const Quaternion q(axis, radian);
	const Matrix44 mat = q.GetMatrix();

	Vector3 v = m_eyePos - m_lookAt;
	v *= mat;
	m_eyePos = m_lookAt + v;

	UpdateViewMatrix();
}


// Up 축으로 회전한다.
void cCamera3D::Yaw2(const float radian, const Vector3 &up)
{
	RET(radian == 0);

	const Quaternion q(up, radian);
	const Matrix44 mat = q.GetMatrix();

	Vector3 v = m_eyePos - m_lookAt;
	v *= mat;
	m_eyePos = m_lookAt + v;
	m_up = up;

	UpdateViewMatrix();
}


// Direction 축으로 회전한다.
void cCamera3D::Roll2(const float radian)
{
	RET(radian == 0);

	const Vector3 axis = GetDirection();
	const Quaternion q(axis, radian);
	const Matrix44 mat = q.GetMatrix();

	Vector3 v = m_lookAt - m_eyePos;
	v *= mat;
	m_lookAt = m_eyePos + v;

	UpdateViewMatrix();
}


// orbit moving
void cCamera3D::Pitch3(const float radian, const Vector3 &target)
{
	RET(radian == 0);

	const Vector3 axis = GetRight();
	const Quaternion q(axis, radian);
	const Matrix44 mat = q.GetMatrix();

	{
		Vector3 toEye = m_eyePos - target;
		toEye *= mat;
		// Vertical Direction BugFix
		if (abs(Vector3(0, 1, 0).DotProduct(toEye.Normal())) > 0.98f)
			return;
		m_eyePos = target + toEye;
	}

	{
		//Vector3 toLookat = m_lookAt - target;
		//toLookat *= mat;
		//m_lookAt = target + toLookat;
		//m_lookAt = target;
	}

	UpdateViewMatrix();
}


// orbit moving
void cCamera3D::Yaw3(const float radian, const Vector3 &target)
{
	RET(radian == 0);

	const Vector3 axis = GetUpVector();
	const Quaternion q(axis, radian);
	const Matrix44 mat = q.GetMatrix();

	{
		Vector3 toEye = m_eyePos - target;
		toEye *= mat;
		m_eyePos = target + toEye;
	}

	{
		//Vector3 toLookat = m_lookAt - target;
		//toLookat *= mat;
		//m_lookAt = target + toLookat;
		//m_lookAt = target;
	}

	UpdateViewMatrix();
}


void cCamera3D::Roll3(const float radian, const Vector3 &target)
{
	RET(radian == 0);

	//const Vector3 axis = GetDirection();
	//const Quaternion q(axis, radian);
	//const Matrix44 mat = q.GetMatrix();

	//Vector3 v = target - m_eyePos;
	//v *= mat;
	//target = m_eyePos + v;

	//UpdateViewMatrix();
}


void cCamera3D::Yaw4(const float radian, const Vector3 &up, const Vector3 &target)
{
	RET(radian == 0);

	const Quaternion q(up, radian);
	const Matrix44 mat = q.GetMatrix();

	{
		Vector3 v = m_eyePos - target;
		v *= mat;
		m_eyePos = target + v;
	}

	{
		Vector3 v = m_lookAt - target;
		v *= mat;
		m_lookAt = target + v;
	}

	m_up = up;

	UpdateViewMatrix();
}


void cCamera3D::Pitch4(const float radian, const Vector3 &up, const Vector3 &target)
{
	RET(radian == 0);

	const Vector3 right = up.CrossProduct(GetDirection()).Normal();
	const Quaternion q(right, radian);
	const Matrix44 mat = q.GetMatrix();

	Vector3 eyePos;
	{
		Vector3 v = m_eyePos - target;
		v *= mat;
		eyePos = target + v;
	}

	Vector3 lookAt;
	{
		Vector3 v = m_lookAt - target;
		v *= mat;
		lookAt = target + v;
	}

	Vector3 dir = lookAt - eyePos;
	dir.Normalize();

	// Vertical Direction BugFix
	if (abs(Vector3(0, 1, 0).DotProduct(dir)) > 0.98f)
		return;

	m_lookAt = lookAt;
	m_eyePos = eyePos;
	m_up = up;

	UpdateViewMatrix();
}


void cCamera3D::KeepHorizontal()
{
	Vector3 dir = m_lookAt - m_eyePos;
	dir.Normalize();
	Vector3 right = Vector3(0, 1, 0).CrossProduct(dir);
	right.Normalize();
	Vector3 up = dir.CrossProduct(right);
	up.Normalize();
	m_up = up;
}


// lookAt - eyePos 사이 거리
float cCamera3D::GetDistance() const
{
	return (m_lookAt - m_eyePos).Length();
}


// Direction 단위 벡터를 리턴한다.
Vector3 cCamera3D::GetDirection() const
{
	Vector3 v = m_lookAt - m_eyePos;
	return v.Normal();
}


// Right 단위 벡터를 리턴한다.
Vector3 cCamera3D::GetRight() const
{
	Vector3 v = m_up.CrossProduct(GetDirection());
	return v.Normal();
}


Vector3 cCamera3D::GetUpVector() const 
{ 
	return GetDirection().CrossProduct(GetRight()).Normal(); 
}


// 카메라 프러스텀 영역크기를 리턴한다.
cBoundingSphere cCamera3D::GetBoundingSphere() const
{
	return GetBoundingSphere(m_near, m_far);
}


// 카메라 프러스텀 영역크기를 리턴한다.
cBoundingSphere cCamera3D::GetBoundingSphere(const float nearPlane, const float farPlane) const 
{
	const float fTanFOVX = tanf(m_aspect * m_fov);
	const float fTanFOVY = tanf(m_aspect);

	const Vector3 dir = GetDirection();
	const Vector3 right = Vector3(0, 1, 0).CrossProduct(dir).Normal();
	const Vector3 up = dir.CrossProduct(right).Normal();
	const Vector3 center = m_eyePos + dir * (nearPlane + 0.5f * (nearPlane + farPlane));
	const Vector3 boundSpan = m_eyePos
		+ (-right * fTanFOVX + up * fTanFOVY + dir) * farPlane - center;

	const float radius = boundSpan.Length();
	return cBoundingSphere(center, radius);
}
