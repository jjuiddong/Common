
#include "stdafx.h"
#include "camera.h"


using namespace graphic;

cCamera::cCamera() :
	m_eyePos(0,100,-100)
,	 m_isOrthogonal(false)
,	m_lookAt(0,0,0)
,	m_up(0,1,0)
, m_renderer(NULL)
, m_oldWidth(0)
, m_oldHeight(0)
, m_width(0)
, m_height(0)
, m_state(eState::STOP)
{
	UpdateViewMatrix();
}

cCamera::cCamera(const Vector3 &eyePos, const Vector3 &lookAt, const Vector3 &up) :
	m_eyePos(eyePos)
,	m_lookAt(lookAt)
,	m_up(up)
, m_renderer(NULL)
{
	UpdateViewMatrix();
}

cCamera::~cCamera()
{
}


// 카메라 파라메터를 초기화하고, 카메라 행렬을 만든다.
void cCamera::SetCamera(const Vector3 &eyePos, const Vector3 &lookAt, const Vector3 &up)
{
	m_eyePos = eyePos;
	m_lookAt = lookAt;
	m_up = up;
	UpdateViewMatrix();
}


void cCamera::SetProjection(const float fov, const float aspect, const float nearPlane, const float farPlane)
{
	m_isOrthogonal = false;
	m_fov = fov;
	m_aspect = aspect;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	m_proj.SetProjection(fov, aspect, nearPlane, farPlane);
	UpdateProjectionMatrix();
}


void cCamera::SetProjectionOrthogonal(const float width, const float height, const float nearPlane, const float farPlane)
{
	//m_fov = fov;
	m_isOrthogonal = true;
	m_aspect = height / width;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	m_width = (int)width;
	m_height = (int)height;
	m_proj.SetProjectionOrthogonal(width, height, nearPlane, farPlane);
	UpdateProjectionMatrix();
}


void cCamera::ReCalcProjection(const float nearPlane, const float farPlane)
{
	if (m_isOrthogonal)
	{
		SetProjectionOrthogonal((float)m_width, (float)m_height, nearPlane, farPlane);
	}
	else
	{
		SetProjection(m_fov, m_aspect, nearPlane, farPlane);
	}
}


// Direction 단위 벡터를 리턴한다.
Vector3 cCamera::GetDirection() const 
{
	Vector3 v = m_lookAt - m_eyePos;
	return v.Normal();
}


// Right 단위 벡터를 리턴한다.
Vector3 cCamera::GetRight() const
{
	Vector3 v = m_up.CrossProduct( GetDirection() );
	return v.Normal();
}


void cCamera::Update(const float deltaSeconds)
{
	RET(eState::STOP == m_state);
	if (m_mover.empty())
	{
		m_state = eState::STOP;
		return;
	}

	// PID
	const float Ki = 8.f;
	const float Kd = 5.f;
	const sCamMoving &mover = m_mover.front();

	// eyepos move
	int check = 0;
	static Vector3 oldPosDir;
	Vector3 posDir = mover.eyePos - m_eyePos;
	const float len = posDir.Length();
	if (len > 0.5f)
	{
		Vector3 dv;
		if (!oldPosDir.IsEmpty())
			dv = posDir - oldPosDir;
		oldPosDir = posDir;

		m_eyePos += posDir * deltaSeconds * Ki + (dv * deltaSeconds * Kd);
	}
	else
	{
		++check;
	}

	// lookat move
	static Vector3 oldLookAtDir;
	Vector3 lookDir = mover.lookAt - m_lookAt;
	const float len2 = lookDir.Length();
	if (len2 > 0.5f)
	{
		Vector3 dv;
		if (!oldLookAtDir.IsEmpty())
			dv = lookDir - oldLookAtDir;
		oldLookAtDir = lookDir;

		m_lookAt += lookDir * deltaSeconds * Ki + (dv * deltaSeconds * Kd);
	}
	else
	{
		++check;
	}

	if (check >= 2)
		rotatepopvector(m_mover, 0);

	KeepHorizontal();
	UpdateViewMatrix();	
}


// Rendering Camera Position, Direction
void cCamera::Render(cRenderer &renderer)
{
	m_lines[0].SetLine(renderer, m_eyePos, m_eyePos + GetRight(), 0.1f, D3DCOLOR_XRGB(255,0,0) );
	m_lines[1].SetLine(renderer, m_eyePos, m_eyePos + GetUpVector(), 0.1f, D3DCOLOR_XRGB(0, 255, 0));
	m_lines[2].SetLine(renderer, m_eyePos, m_eyePos + GetDirection(), 0.1f, D3DCOLOR_XRGB(0, 0, 255));

	for (auto &line : m_lines)
		line.Render(renderer);
}


// shader bind
void cCamera::Bind(cShader &shader)
{
	shader.SetCameraView(m_view);
	shader.SetCameraProj(m_proj);
	shader.SetCameraEyePos(m_eyePos);
}


void cCamera::Bind(cRenderer &renderer)
{
	renderer.GetDevice()->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&m_view);
	renderer.GetDevice()->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_proj);
}


void cCamera::UpdateViewMatrix(const bool updateUp)
// updateUp = true
{
	RET(!m_renderer);

	m_view.SetView2(m_eyePos, m_lookAt, m_up);

	// Update Up Vector
	//if (updateUp)
	//{
	//	Vector3 dir = m_lookAt - m_eyePos;
	//	dir.Normalize();
	//	Vector3 right = m_up.CrossProduct(dir);
	//	right.Normalize();
	//	m_up = dir.CrossProduct(right);
	//	m_up.Normalize();
	//}
	//

	if (m_renderer->GetDevice())
		m_renderer->GetDevice()->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&m_view);
}


// ViewMatrix 로 eyePos, lookAt, up 을 계산한다.
void cCamera::UpdateParameterFromViewMatrix()
{
	const Vector3 right(m_view._11, m_view._21, m_view._31);
	const Vector3 up(m_view._12, m_view._22, m_view._32 );
	const Vector3 dir(m_view._13, m_view._23, m_view._33);
	const Vector3 pos = m_view.Inverse().GetPosition();
	
	m_eyePos = pos;
	m_up = up;
	m_lookAt = dir * 100 + pos;
}


void cCamera::UpdateProjectionMatrix()
{
	RET(!m_renderer);

	if (m_renderer->GetDevice())
		m_renderer->GetDevice()->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_proj);
}


// Right 축으로 회전한다.
void cCamera::Pitch( const float radian )
{
	RET(radian == 0);

	const Vector3 axis = GetRight();
	const Quaternion q( axis, radian );
	const Matrix44 mat = q.GetMatrix();

	Vector3 v = m_lookAt - m_eyePos;
	v *= mat;
	m_lookAt = m_eyePos + v;

	UpdateViewMatrix();
}


// Up 축으로 회전한다.
void cCamera::Yaw( const float radian )
{
	RET(radian == 0);

	const Vector3 axis = GetUpVector();
	const Quaternion q( axis, radian );
	const Matrix44 mat = q.GetMatrix();

	Vector3 v = m_lookAt - m_eyePos;
	v *= mat;
	m_lookAt = m_eyePos + v;

	UpdateViewMatrix();	
}


// Direction 축으로 회전한다.
void cCamera::Roll( const float radian )
{
	RET(radian == 0);

	const Vector3 axis = GetDirection();
	const Quaternion q( axis, radian );
	const Matrix44 mat = q.GetMatrix();

	Vector3 v = m_lookAt - m_eyePos;
	v *= mat;
	m_lookAt = m_eyePos + v;

	UpdateViewMatrix();	
}



// Right 축으로 회전한다.
void cCamera::Pitch2( const float radian)
{
	RET(radian == 0);

	const Vector3 axis = GetRight();
	const Quaternion q( axis, radian );
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
void cCamera::Pitch2(const float radian, const Vector3 &up)
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
void cCamera::Yaw2( const float radian)
{
	RET(radian == 0);

	const Vector3 axis = GetUpVector();
	const Quaternion q( axis, radian );
	const Matrix44 mat = q.GetMatrix();

	Vector3 v = m_eyePos - m_lookAt;
	v *= mat;
	m_eyePos = m_lookAt + v;

	UpdateViewMatrix();
}


// Up 축으로 회전한다.
void cCamera::Yaw2(const float radian, const Vector3 &up)
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
void cCamera::Roll2( const float radian)
{
	RET(radian == 0);

	const Vector3 axis = GetDirection();
	const Quaternion q( axis, radian );
	const Matrix44 mat = q.GetMatrix();

	Vector3 v = m_lookAt - m_eyePos;
	v *= mat;
	m_lookAt = m_eyePos + v;

	UpdateViewMatrix();
}


void cCamera::Pitch3(const float radian, const Vector3 &target)
{
	RET(radian == 0);

	const Vector3 axis = GetRight();
	const Quaternion q(axis, radian);
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

	UpdateViewMatrix();
}


void cCamera::Yaw3(const float radian, const Vector3 &target)
{
	RET(radian == 0);

	const Vector3 axis = GetUpVector();
	const Quaternion q(axis, radian);
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

	UpdateViewMatrix();
}


void cCamera::Roll3(const float radian, const Vector3 &target)
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


void cCamera::Yaw4(const float radian, const Vector3 &up, const Vector3 &target)
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


void cCamera::Pitch4(const float radian, const Vector3 &up, const Vector3 &target)
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


void cCamera::KeepHorizontal()
{
	Vector3 dir = m_lookAt - m_eyePos;
	dir.Normalize();
	Vector3 right = Vector3(0, 1, 0).CrossProduct(dir);
	right.Normalize();
	Vector3 up = dir.CrossProduct(right);
	up.Normalize();
	m_up = up;
}


void cCamera::Move(const Vector3 &eyePos, const Vector3 &lookAt)
{
	// Initialize mover vector
	m_mover.clear();

	sCamMoving info;
	info.eyePos = eyePos;
	info.lookAt = lookAt;

	const float eyePosVelocity = (eyePos - m_eyePos).Length();
	const float lookAtVelocity = (lookAt - m_lookAt).Length();

	info.velocityLookAt = lookAtVelocity;
	info.velocityPos = eyePosVelocity;

	m_mover.push_back(info);
	m_state = eState::MOVE;
}


void cCamera::MoveNext(const Vector3 &eyePos, const Vector3 &lookAt)
{
	sCamMoving info;
	info.eyePos = eyePos;
	info.lookAt = lookAt;

	Vector3 curEyePos = (m_mover.empty()) ? m_eyePos : m_mover.back().eyePos;
	Vector3 curLookAt = (m_mover.empty()) ? m_lookAt : m_mover.back().lookAt;

	const float eyePosVelocity = (eyePos - curEyePos).Length();
	const float lookAtVelocity = (lookAt - curLookAt).Length();

	info.velocityLookAt = lookAtVelocity;
	info.velocityPos = eyePosVelocity;

	m_mover.push_back(info);
	m_state = eState::MOVE;
}


void cCamera::MoveCancel()
{
	m_state = eState::STOP;
}


// 앞/뒤으로 이동
void cCamera::MoveFront( const float len )
{
	const Vector3 dir = GetDirection();
	m_lookAt += dir * len;
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


void cCamera::MoveFrontHorizontal(const float len)
{
	Vector3 dir = GetDirection();
	dir.y = 0;
	dir.Normalize();
	m_lookAt += dir * len;
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


void cCamera::Move(const cBoundingBox &bbox)
{
	Vector3 dir = (bbox.m_min - bbox.Center()) * 5.f;
	Vector3 pos = dir + bbox.m_min;
	pos.y = bbox.m_max.y;
	SetCamera(pos, bbox.Center(), Vector3(0, 1, 0));
}


// 위아래 이동
void cCamera::MoveUp( const float len )
{
	const Vector3 dir = GetUpVector();
	m_lookAt += dir * len;
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


// 좌우 이동.
void cCamera::MoveRight( const float len )
{
	const Vector3 dir = GetRight();
	m_lookAt += dir * len;
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


// dir 방향으로 이동한다.
void cCamera::MoveAxis( const Vector3 &dir, const float len )
{
	m_lookAt += dir * len;
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


// lookAt 은 고정된채로 eyePos 를 이동한다.
void cCamera::Zoom( const float len)
{
	Zoom(GetDirection(), len);
}

void cCamera::Zoom(const Vector3 &dir, const float len)
{
	m_eyePos += dir * len;
	m_lookAt += dir * len;
	UpdateViewMatrix();
}


// lookAt - eyePos 사이 거리
float cCamera::GetDistance() const 
{
	return (m_lookAt - m_eyePos).Length();
}


Vector3 cCamera::GetScreenPos(const Vector3& vPos)
{
	return GetScreenPos(m_width, m_height, vPos);
}


Vector3 cCamera::GetScreenPos(const int viewportWidth, const int viewportHeight, const Vector3& vPos)
{
// 	Vector3 point = vPos * m_view * m_proj;
// 	point.x = (point.x + 1) * viewportWidth / 2;
// 	point.y = (-point.y + 1) * viewportHeight / 2;

	D3DVIEWPORT9 viewPort;
	viewPort.X = 0;
	viewPort.Y = 0;
	viewPort.Width = viewportWidth;
	viewPort.Height = viewportHeight;
	viewPort.MinZ = 0;
	viewPort.MaxZ = 1;

	Matrix44 world;

	Vector3 point;
	D3DXVec3Project((D3DXVECTOR3*)&point, (const D3DXVECTOR3*)&vPos, 
		&viewPort, (const D3DXMATRIX*)&m_proj, (const D3DXMATRIX*)&m_view, 
		(const D3DXMATRIX*)&world);

	return point;
}


void cCamera::GetRay(OUT Vector3 &orig, OUT Vector3 &dir)
{
	GetRay(m_width, m_height, m_width/2, m_height/2, orig, dir);
}


void cCamera::GetRay(const int sx, const int sy, OUT Vector3 &orig, OUT Vector3 &dir)
{
	GetRay(m_width, m_height, sx, sy, orig, dir);
}


void cCamera::GetRay(const int windowWidth, const int windowHeight, 
	const int sx, const int sy, OUT Vector3 &orig, OUT Vector3 &dir)
{
	const float x = ((sx * 2.0f / windowWidth) - 1.0f);
	const float y = -((sy * 2.0f / windowHeight) - 1.0f);

	Vector3 v;
	v.x = x / m_proj._11;
	v.y = y / m_proj._22;
	v.z = 1.0f;

	Matrix44 m = m_view.Inverse();

	dir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	dir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	dir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
	dir.Normalize();

	orig.x = m._41;
	orig.y = m._42;
	orig.z = m._43;

	orig += (dir * 1);
}


// return camera position, shadow matrix
void cCamera::GetShadowMatrix(OUT Matrix44 &view, OUT Matrix44 &proj, OUT Matrix44 &tt)
{
	view = GetViewMatrix();
	proj = GetProjectionMatrix();

	D3DXMATRIX mTT = D3DXMATRIX(0.5f, 0.0f, 0.0f, 0.0f
		, 0.0f, -0.5f, 0.0f, 0.0f
		, 0.0f, 0.0f, 1.0f, 0.0f
		, 0.5f, 0.5f, 0.0f, 1.0f);
	tt = *(Matrix44*)&mTT;
}


// use ShadowMap, Calc Light Camera Projection
// Light Camera is Orthogonal Projection
// this class is Light Camera instance
void cCamera::FitFrustum(const cCamera &camera
	, const float farPlaneRate//= 1.f
)
{
	// Camera View Space
	Matrix44 oldProj;
	oldProj.SetProjection(camera.m_fov, camera.m_aspect, camera.m_nearPlane, camera.m_farPlane * farPlaneRate);
	FitFrustum(camera.m_view * oldProj);
}


// use ShadowMap, Calc Light Camera Projection
// Light Camera is Orthogonal Projection
// this class is Light Camera instance
void cCamera::FitFrustum(const Matrix44 &matViewProj)
{
	//        4 --- 5
	//      / |  |  /|
	//   0 --- 1   |
	//   |   6-|- -7
	//   | /     | /
	//   2 --- 3
	//
	Vector3 vertices[8] = {
		Vector3(-1,1,0), Vector3(1,1,0), Vector3(-1,-1,0), Vector3(1,-1,0),
		Vector3(-1,1, 1), Vector3(1,1, 1), Vector3(-1,-1,1), Vector3(1,-1,1),
	};

	// Camera View Space
	Matrix44 matInv = matViewProj.Inverse();
	for (int i = 0; i < 8; i++)
		vertices[i] *= matInv;

	Vector3 center(0, 0, 0);
	for (int i = 0; i < 8; ++i)
		center += vertices[i];
	center /= 8.f;

	//const float distFromCenter = 500;
	const float distFromCenter = 50;
	const Vector3 pos = center - (GetDirection()*distFromCenter);
	Matrix44 newView;
	newView.SetView2(pos, center, Vector3(0, 1, 0));

	// Light View Space
	for (int i = 0; i < 8; i++)
		vertices[i] *= newView;

	sMinMax mm;
	for each (auto &v in vertices)
		mm.Update(v);

	Matrix44 newProj;
	newProj.SetProjectionOrthogonal(
		mm._min.x, mm._max.x,
		mm._min.y, mm._max.y,
		mm._min.z, mm._max.z);

	m_eyePos = pos;
	m_lookAt = center;
	m_view = newView;
	m_proj = newProj;
	m_nearPlane = mm._min.z;
	m_farPlane = mm._max.z;
	m_viewProj = newView * newProj;
}


void cCamera::FitQuad(const Vector3 vertices[4])
{
	Vector3 center(0, 0, 0);
	for (int i = 0; i < 4; ++i)
		center += vertices[i];
	center /= 4.f;

	//const float distFromCenter = 500;
	const float distFromCenter = 50;
	const Vector3 pos = center - (GetDirection()*distFromCenter);
	Matrix44 newView;
	newView.SetView2(pos, center, Vector3(0, 1, 0));

	// Light View Space
	Vector3 quad[4];
	for (int i = 0; i < 4; i++)
		quad[i] = vertices[i] * newView;

	sMinMax mm;
	for each (auto &v in quad)
		mm.Update(v);

	Matrix44 newProj;
	newProj.SetProjectionOrthogonal(
		mm._min.x, mm._max.x,
		mm._min.y, mm._max.y,
		mm._min.z, mm._max.z);

	m_eyePos = pos;
	m_lookAt = center;
	m_view = newView;
	m_proj = newProj;
	m_nearPlane = mm._min.z;
	m_farPlane = mm._max.z;
	m_viewProj = newView * newProj;
}


void cCamera::SetViewPort(const int width, const int height)
{
	m_oldWidth = m_width;
	m_oldHeight = m_height;
	m_width = width;
	m_height = height;

	if (m_isOrthogonal)
	{
		SetProjectionOrthogonal((float)width, (float)height
			, m_nearPlane, m_farPlane);
	}
	else
	{
		SetProjection(m_fov, (float)width / (float)height
			, m_nearPlane, m_farPlane);
	}
}
