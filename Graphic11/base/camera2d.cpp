
#include "stdafx.h"
#include "camera2d.h"

using namespace graphic;


cCamera2D::cCamera2D(const char *name)
	: cCamera(name)
	, m_zoom(1.f)
	, m_maxZoom(10.f)
{
	m_up = Vector3(0, 0, 1); // Camera Up Vector (Z Axis)
	UpdateViewMatrix();
}

cCamera2D::cCamera2D(const char *name, const Vector3 &eyePos, const Vector3 &up)
	: cCamera(name)
{
	m_eyePos = eyePos;
	m_up = up;
	UpdateViewMatrix();
}

cCamera2D::~cCamera2D()
{
}


bool cCamera2D::Is2DMode() const
{
	return true;
}


void cCamera2D::SetProjectionOrthogonal(const float width, const float height
	, const float nearPlane, const float farPlane)
{
	m_aspect = height / width;
	m_near = nearPlane;
	m_far = farPlane;
	m_width = width;
	m_height = height;
	m_proj.SetProjectionOrthogonal(width, height, nearPlane, farPlane);
}


void cCamera2D::ReCalcProjection(const float nearPlane, const float farPlane)
{
	SetProjectionOrthogonal(m_width, m_height, nearPlane, farPlane);
}


// Right 단위 벡터를 리턴한다.
Vector3 cCamera2D::GetRight() const
{
	Vector3 v = m_up.CrossProduct(GetDirection());
	return v.Normal();
}


void cCamera2D::Update(const float deltaSeconds)
{
	__super::Update(deltaSeconds);
}


void cCamera2D::Bind(cRenderer &renderer)
{
	Matrix44 zoom;
	zoom.SetScale(Vector3(1, 1, 1));
	const XMMATRIX mView = XMLoadFloat4x4((XMFLOAT4X4*)&m_view);
	const XMMATRIX mProj = XMLoadFloat4x4((XMFLOAT4X4*)&m_proj);
	renderer.m_cbPerFrame.m_v->mView = XMMatrixTranspose(zoom.GetMatrixXM()*mView);
	renderer.m_cbPerFrame.m_v->mProjection = XMMatrixTranspose(mProj);
	renderer.m_cbPerFrame.m_v->eyePosW = m_eyePos;
}


void cCamera2D::UpdateViewMatrix()
{
	m_view.SetView(m_eyePos, Vector3(0,-1,0), m_up);
}


// ViewMatrix 로 eyePos, lookAt, up 을 계산한다.
void cCamera2D::UpdateParameterFromViewMatrix()
{
	const Vector3 right(m_view._11, m_view._21, m_view._31);
	const Vector3 up(m_view._12, m_view._22, m_view._32);
	const Vector3 dir(m_view._13, m_view._23, m_view._33);
	const Vector3 pos = m_view.Inverse().GetPosition();

	m_eyePos = pos;
	m_up = up;
	//m_lookAt = dir * 100 + pos;
}


// Y축으로 회전한다.
void cCamera2D::Roll(const float radian)
{
	RET(radian == 0);

	const Vector3 axis = GetDirection();
	const Quaternion q(axis, radian);
	const Matrix44 mat = q.GetMatrix();

	m_up *= mat;

	UpdateViewMatrix();
}


// Direction 축으로 회전한다.
void cCamera2D::Roll2(const float radian)
{
	RET(radian == 0);

	const Vector3 axis = GetDirection();
	const Quaternion q(axis, radian);
	const Matrix44 mat = q.GetMatrix();

	//Vector3 v = m_lookAt - m_eyePos;
	//v *= mat;
	//m_lookAt = m_eyePos + v;

	UpdateViewMatrix();
}


void cCamera2D::Move(const Vector3 &eyePos, const Vector3&)
{
	// Initialize mover vector
	m_mover.clear();

	sCamMoving info;
	info.eyePos = eyePos;
	info.lookAt = Vector3(eyePos.x, 0, eyePos.z);

	const float eyePosVelocity = (eyePos - m_eyePos).Length();
	const float lookAtVelocity = (eyePos - m_eyePos).Length();

	info.velocityLookAt = lookAtVelocity;
	info.velocityPos = eyePosVelocity;

	m_mover.push_back(info);
	m_state = eState::MOVE;
}


void cCamera2D::MoveNext(const Vector3 &eyePos, const Vector3&)
{
	const Vector3 lookAt(eyePos.x, 0, eyePos.z);

	sCamMoving info;
	info.eyePos = eyePos;
	info.lookAt = lookAt;

	Vector3 curEyePos = (m_mover.empty()) ? m_eyePos : m_mover.back().eyePos;
	Vector3 curLookAt = (m_mover.empty()) ? lookAt : m_mover.back().lookAt;

	const float eyePosVelocity = (eyePos - curEyePos).Length();
	const float lookAtVelocity = (lookAt - curLookAt).Length();

	info.velocityLookAt = lookAtVelocity;
	info.velocityPos = eyePosVelocity;

	m_mover.push_back(info);
	m_state = eState::MOVE;
}


// 앞/뒤으로 이동
void cCamera2D::MoveFront(const float len)
{
	const Vector3 dir = GetDirection();
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


void cCamera2D::Move(const cBoundingBox &bbox)
{
	const Vector3 dim = bbox.GetDimension();
	const float m = max(dim.x, max(dim.y, dim.z)) * 1.5f;
	Vector3 pos(-m, m, -m);
	SetCamera(pos, Vector3(0, 0, 1));
}


// UpVector 축으로 이동한다.
void cCamera2D::MoveUp(const float len)
{
	const Vector3 dir = GetUpVector();
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


// RightVector 축으로 이동한다.
void cCamera2D::MoveRight(const float len)
{
	const Vector3 dir = GetRight();
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


// dir 방향으로 이동한다.
void cCamera2D::MoveAxis(const Vector3 &dir, const float len)
{
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


// lookAt 은 고정된채로 eyePos 를 이동한다.
void cCamera2D::Zoom(const float len)
{
	m_zoom = min(m_maxZoom, m_zoom * len);
}

void cCamera2D::Zoom(const Vector3 &dir, const float len)
{
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


// eyePos 높이를 리턴한다.
float cCamera2D::GetDistance() const
{
	return m_eyePos.y;
}


Vector3 cCamera2D::GetScreenPos(const Vector3& vPos)
{
	return GetScreenPos((int)m_width, (int)m_height, vPos);
}


Vector3 cCamera2D::GetScreenPos(const int viewportWidth, const int viewportHeight, const Vector3& vPos)
{
	assert(0);
	return Vector3(0, 0, 0);
}


Ray cCamera2D::GetRay(
	const int sx //=-1
	, const int sy //=-1
)
{
	assert(m_width != 0);
	assert(m_height != 0);

	const int x = (sx == -1) ? (int)m_width / 2 : sx;
	const int y = (sy == -1) ? (int)m_height / 2 : sy;

	Ray ray;
	ray.dir = Vector3(0, -1, 0);
	ray.orig = m_eyePos;
	
	Vector3 offset((float)x - (m_width / 2), 0, (m_height / 2) - (float)y);

	Quaternion q;
	q.SetRotationArc(Vector3(0, 0, 1), m_up);
	const Matrix44 mat = q.GetMatrix();
	offset *= mat;

	ray.orig += offset;

	//GetRay((int)m_width, (int)m_height, x, y, ray.orig, ray.dir);
	return ray;
}


void cCamera2D::GetRay(const int windowWidth, const int windowHeight,
	const int sx, const int sy, OUT Vector3 &orig, OUT Vector3 &dir)
{
	const float x = ((sx * 2.0f / windowWidth) - 1.0f);
	const float y = -((sy * 2.0f / windowHeight) - 1.0f);

	//Vector3 v;
	//v.x = x / m_proj._11;
	//v.y = y / m_proj._22;
	//v.z = 1.0f;
	Vector3 v(x, y, 1);
	v *= m_proj.Inverse();

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
void cCamera2D::GetShadowMatrix(OUT Matrix44 &view, OUT Matrix44 &proj, OUT Matrix44 &tt) const
{
	view = GetViewMatrix();
	proj = GetProjectionMatrix();

	Matrix44 mTT(0.5f, 0.0f, 0.0f, 0.0f
		, 0.0f, -0.5f, 0.0f, 0.0f
		, 0.0f, 0.0f, 1.0f, 0.0f
		, 0.5f, 0.5f, 0.0f, 1.0f);
	tt = *(Matrix44*)&mTT;
}


// use ShadowMap, Calc Light Camera Projection
// Light Camera is Orthogonal Projection
// this class is Light Camera instance
void cCamera2D::FitFrustum(const cCamera &camera
	, const float farPlaneRate//= 1.f
)
{
	// Camera View Space
	Matrix44 oldProj;
	oldProj.SetProjection(camera.m_fov, camera.m_aspect, camera.m_near, camera.m_far* farPlaneRate);
	FitFrustum(camera.m_view * oldProj);
}


// use ShadowMap, Calc Light Camera Projection
// Light Camera is Orthogonal Projection
// this class is Light Camera instance
void cCamera2D::FitFrustum(const Matrix44 &matViewProj)
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
		mm._min.z, mm._max.z + 100);

	m_eyePos = pos;
	//m_lookAt = center;
	m_view = newView;
	m_proj = newProj;
	m_near = mm._min.z;
	m_far = mm._max.z;
	m_viewProj = newView * newProj;
}


void cCamera2D::FitQuad(const Vector3 vertices[4])
{
	Vector3 center(0, 0, 0);
	for (int i = 0; i < 4; ++i)
		center += vertices[i];
	center /= 4.f;

	const float distFromCenter = 500;
	//const float distFromCenter = 50;
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
		mm._min.z - 100, mm._max.z + 100);

	m_eyePos = pos;
	//m_lookAt = center;
	m_view = newView;
	m_proj = newProj;
	m_near = mm._min.z;
	m_far = mm._max.z;
	m_viewProj = newView * newProj;
}


void cCamera2D::SetViewPort(const float width, const float height)
{
	__super::SetViewPort(width, height);
	SetProjectionOrthogonal(width, height, m_near, m_far);
}


Matrix44 cCamera2D::GetZoomMatrix() const
{
	Matrix44 ret;
	ret.SetScale(Vector3(1, 1, 1)*m_zoom);
	return ret;
}


Vector3 cCamera2D::GetLookAt() const
{
	return Vector3(m_eyePos.x, 0, m_eyePos.z);
}


Vector3 cCamera2D::GetUpVector() const 
{ 
	return m_up; 
}

void cCamera2D::SetUpVector(const Vector3 &up) 
{ 
	m_up = up;
	UpdateViewMatrix(); 
}
