
#include "stdafx.h"
#include "camera.h"


using namespace graphic;

cCamera::cCamera(const char *name) 
	: m_name(name)
	, m_eyePos(0,100,-100)
	, m_lookAt(0, 0, 0)
	, m_up(0, 1, 0)
	, m_isOrthogonal(false)
	, m_aspect(1)
	, m_fov(MATH_PI / 4.f)
	, m_width(0)
	, m_height(0)
	, m_oldWidth(0)
	, m_oldHeight(0)
	, m_state(eState::STOP)
	, m_boundingType(eBoundingType::NONE)
	, m_traceNode(NULL)
	, m_isKeepHorizontal(true)
	, m_kp(4.f) // 8.f
	, m_kd(0.f) // 5.f
{
	m_boundingHSphere.SetBoundingHalfSphere(Vector3(0, 0, 0), 500);
}

cCamera::~cCamera()
{
}


bool cCamera::Is2DMode() const
{
	return false;
}


void cCamera::SetCamera(const Vector3 &eyePos, const Vector3 &up)
{
	m_eyePos = eyePos;
	m_lookAt = Vector3(eyePos.x, 0, eyePos.z);
	m_up = up;
	UpdateViewMatrix();
}


// 카메라 파라메터를 초기화하고, 카메라 행렬을 만든다.
void cCamera::SetCamera(const Vector3 &eyePos, const Vector3 &lookAt, const Vector3 &up)
{
	m_eyePos = eyePos;
	m_lookAt = lookAt;
	m_up = up;
	UpdateViewMatrix();
}


void cCamera::SetCamera2(const Vector3 &eyePos, const Vector3 &direction, const Vector3 &up)
{
	m_eyePos = eyePos;
	m_lookAt = direction * 10 + eyePos;
	m_up = up;
	UpdateViewMatrix();
}


// 카메라 움직임 애니메이션 처리
void cCamera::Update(const float deltaSeconds)
{
	UpdateTrace(deltaSeconds);
	UpdateMove(deltaSeconds);
	CheckBoundingBox();
}


// 카메라 이동에 관련된 작업을 한다.
void cCamera::UpdateMove(const float deltaSeconds)
{
	RET(eState::MOVE != m_state);
	if (m_mover.empty())
	{
		m_state = eState::STOP;
		return;
	}

	const float dt = min(deltaSeconds, 0.05f);

	const sCamMoving &mover = m_mover.front();

	// eyepos move
	int check = 0;
	m_oldPosDir;
	Vector3 posDir = mover.eyePos - m_eyePos;
	const float len = posDir.Length();
	if (len > 1.f)
	{
		if (mover.velocityPos == 0.f)
		{
			// PID Move
			Vector3 dv;
			if (!m_oldPosDir.IsEmpty())
				dv = posDir - m_oldPosDir;
			m_oldPosDir = posDir;
			m_eyePos += posDir * dt * m_kp + (dv * dt * m_kd);
		}
		else
		{
			m_eyePos += posDir.Normal() * min(len, mover.velocityPos * dt);
		}
	}
	else
	{
		++check;
	}

	// lookat move
	Vector3 lookDir = mover.lookAt - m_lookAt;
	const float len2 = lookDir.Length();
	if (len2 > 1.f)
	{
		if (mover.velocityLookAt == 0.f)
		{
			// PID Move
			Vector3 dv;
			if (!m_oldLookAtDir.IsEmpty())
				dv = lookDir - m_oldLookAtDir;
			m_oldLookAtDir = lookDir;
			m_lookAt += lookDir * dt * m_kp + (dv * dt * m_kd);
		}
		else
		{
			m_lookAt += lookDir.Normal() * min(len2, mover.velocityLookAt * dt);
		}
	}
	else
	{
		++check;
	}

	if (check >= 2)
		rotatepopvector(m_mover, 0);

	//KeepHorizontal();
	UpdateViewMatrix();
}


// 카메라가 대상을 쫓아갈 때, 위치와 목표 값을 업데이트한다.
void cCamera::UpdateTrace(const float deltaSeconds)
{
	if ((eState::TRAC == m_state) && m_traceNode)
	{
		m_eyePos = m_traceTm.GetPosition() + m_traceNode->GetWorldMatrix().GetPosition();
		m_lookAt = m_traceNode->GetWorldMatrix().GetPosition();
		UpdateViewMatrix();
	}
}


Ray cCamera::GetRay(
	const int sx //=-1
	, const int sy //=-1
) const
{
	assert(m_width != 0);
	assert(m_height != 0);

	const int x = (sx == -1) ? (int)m_width / 2 : sx;
	const int y = (sy == -1) ? (int)m_height / 2 : sy;

	Ray ray;
	GetRay((int)m_width, (int)m_height, x, y, ray.orig, ray.dir);
	return ray;
}


void cCamera::GetRay(const int windowWidth, const int windowHeight, 
	const int sx, const int sy, OUT Vector3 &orig, OUT Vector3 &dir) const
{
	const float x = ((sx * 2.0f / windowWidth) - 1.0f);
	const float y = -((sy * 2.0f / windowHeight) - 1.0f);

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


void cCamera::SetViewPort(const float width, const float height)
{
	m_oldWidth = m_width;
	m_oldHeight = m_height;
	m_width = width;
	m_height = height;

	if (m_isOrthogonal)
	{
		SetProjectionOrthogonal(width, height, m_near, m_far);
	}
	else
	{
		SetProjection(m_fov, width / height, m_near, m_far);
	}
}


void cCamera::MoveCancel()
{
	m_state = eState::STOP;
	m_mover.clear();
}

void cCamera::SetEyePos(const Vector3 &eye) 
{ 
	m_eyePos = eye; 
	UpdateViewMatrix(); 
}
void cCamera::SetLookAt(const Vector3 &lookAt) 
{ 
	m_lookAt = lookAt;
	UpdateViewMatrix(); 
}
void cCamera::SetUpVector(const Vector3 &up) 
{ 
	m_up = up;
	UpdateViewMatrix(); 
}
void cCamera::SetViewMatrix(const Matrix44 &view) 
{ 
	m_view = view; 
}
void cCamera::SetProjectionMatrix(const Matrix44 &proj) 
{ 
	m_proj = proj; 
}

Vector3 cCamera::GetDirection() const
{
	assert(0);
	return Vector3(0, 0, 0);
}

Vector3 cCamera::GetRight() const
{
	assert(0);
	return Vector3(0, 0, 0);
}

float cCamera::GetDistance() const
{
	assert(0);
	return 0;
}

Vector3 cCamera::GetUpVector() const 
{ 
	return m_up; 
}


void cCamera::CheckBoundingBox()
{
	RET(m_boundingType == eBoundingType::NONE);

	// Y축 방향은 양수가되지 않게 한다. (항상 아래를 바라보게 한다.)
	{
		Vector3 dir = GetDirection();
		if (dir.y > -0.1f)
		{
			dir.y = -0.1f;
			dir.Normalize();
			const float len = m_lookAt.Distance(m_eyePos);
			m_eyePos = m_lookAt + -dir * len;
			UpdateViewMatrix();
		}
	}

	// 반구를 벗어나지 않게 한다.
	Vector3 out;
	if (m_boundingHSphere.GetBoundingPoint(m_eyePos, out))
	{
		const Vector3 dir = GetDirection();
		const float len = m_lookAt.Distance(m_eyePos);
		m_eyePos = out;
		m_lookAt = out + dir * len;
		UpdateViewMatrix();
	}
}


void cCamera::Move(const cBoundingBox &bbox)
{
	const Vector3 dim = bbox.GetDimension();
	const float m = max(dim.x, max(dim.y, dim.z)) * 1.5f;
	Vector3 pos(-m, m, -m);
	SetCamera(pos, bbox.Center(), Vector3(0, 1, 0));
}


// 앞/뒤으로 이동
void cCamera::MoveFront(const float len)
{
	const Vector3 dir = GetDirection();
	m_lookAt += dir * len;
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


void cCamera::MoveFrontHorizontal(const float len)
{
	Vector3 dir;
	dir = GetDirection();
	dir.y = 0;
	dir.Normalize();

	m_lookAt += dir * len;
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


// 위아래 이동
void cCamera::MoveUp(const float len)
{
	const Vector3 dir = GetUpVector();
	m_lookAt += dir * len;
	m_eyePos += dir * len;
	UpdateViewMatrix();
}

// 위아래 이동 (only eyePos)
void cCamera::MoveUp2(const float len)
{
	const Vector3 dir = GetUpVector();
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


// 좌우 이동.
void cCamera::MoveRight(const float len)
{
	const Vector3 dir = GetRight();
	m_lookAt += dir * len;
	m_eyePos += dir * len;
	UpdateViewMatrix();
}

// 좌우 이동. (only eyepos)
void cCamera::MoveRight2(const float len)
{
	const Vector3 dir = GetRight();
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


// dir 방향으로 이동한다.
void cCamera::MoveAxis(const Vector3 &dir, const float len)
{
	m_lookAt += dir * len;
	m_eyePos += dir * len;
	UpdateViewMatrix();
}


// 카메라가 trace node를 따라간다.
// tm 변환행렬의 차 만큼 거리를 두고 이동한다.
void cCamera::Trace(cNode *trace, const Matrix44 &tm)
{
	m_traceNode = trace;
	m_traceTm = tm;
	m_state = eState::TRAC;
}


void cCamera::ReCalcProjection(const float nearPlane, const float farPlane)
{
	if (m_isOrthogonal)
	{
		SetProjectionOrthogonal(m_width, m_height, nearPlane, farPlane);
	}
	else
	{
		SetProjection(m_fov, m_aspect, nearPlane, farPlane);
	}
}


void cCamera::SetProjection(const float fov, const float aspect, const float nearPlane, const float farPlane)
{
	m_isOrthogonal = false;
	m_fov = fov;
	m_aspect = aspect;
	m_near = nearPlane;
	m_far = farPlane;
	m_proj.SetProjection(fov, aspect, nearPlane, farPlane);

	m_viewProj = m_view * m_proj;
}


void cCamera::SetProjectionOrthogonal(const float width, const float height, const float nearPlane, const float farPlane)
{
	m_isOrthogonal = true;
	m_aspect = height / width;
	m_near = nearPlane;
	m_far = farPlane;
	m_width = width;
	m_height = height;
	m_proj.SetProjectionOrthogonal(width, height, nearPlane, farPlane);
}


Vector2 cCamera::GetScreenPos(const Vector3& vPos) const
{
	//return GetScreenPos((int)m_width, (int)m_height, vPos);

	Vector3 hPos = vPos * m_viewProj;
	const float x = ((hPos.x + 1.f) / 2.f);
	const float y = 1.f - ((hPos.y + 1.f) / 2.f);
	return Vector2(m_width*x, m_height*y);
}


// return camera position, shadow matrix
void cCamera::GetShadowMatrix(OUT Matrix44 &view, OUT Matrix44 &proj, OUT Matrix44 &tt) const
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
void cCamera::FitFrustum(const cCamera &camera
	, const float farPlaneRate//= 1.f
)
{
	// Camera View Space
	Matrix44 oldProj;
	oldProj.SetProjection(camera.m_fov, camera.m_aspect, camera.m_near, camera.m_far * farPlaneRate);
	FitFrustum(camera.m_view * oldProj);
}


// use ShadowMap, Calc Light Camera Projection
// Light Camera is Orthogonal Projection
// this class is Light Camera Method
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

	if ((mm._min != Vector3::Zeroes)
		|| (mm._max != Vector3::Zeroes))
	{
		Matrix44 newProj;
		newProj.SetProjectionOrthogonal(
			mm._min.x, mm._max.x,
			mm._min.y, mm._max.y,
			mm._min.z, mm._max.z + 100);

		m_proj = newProj;
	}
	else
	{
		// no assume boundngbox
		Matrix44 newProj;
		newProj.SetProjectionOrthogonal(
			-1, 1,
			-1, 1,
			0, 1 + 100.f);
		m_proj = newProj;
	}

	m_eyePos = pos;
	m_lookAt = center;
	m_view = newView;
	m_near = mm._min.z;
	m_far = mm._max.z;
	m_viewProj = m_view * m_proj;
}


void cCamera::FitQuad(const Vector3 vertices[4])
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
	m_lookAt = center;
	m_view = newView;
	m_proj = newProj;
	m_near = mm._min.z;
	m_far = mm._max.z;
	m_viewProj = newView * newProj;
}


// 카메라 프러스텀 영역크기를 리턴한다.
cBoundingSphere cCamera::GetBoundingSphere() const
{
	// 파생되는 클래스에서 구현한다.
	assert(0);
	return{};
}


// 카메라 프러스텀 영역크기를 리턴한다.
cBoundingSphere cCamera::GetBoundingSphere(const float nearPlane, const float farPlane) const
{
	// 파생되는 클래스에서 구현한다.
	assert(0);
	return{};
}


// lookAt 은 고정된채로 eyePos 를 이동한다.
void cCamera::Zoom(const float len)
{
	Zoom(GetDirection(), len);
}

void cCamera::Zoom(const Vector3 &dir, const float len)
{
	// 줌아웃 될때, 카메라 영역을 벗어나는지 체크한다. 벗어나면 무시
	const bool isZoomOut = len < 0;
	if ((m_boundingType == eBoundingType::HALF_SPHERE) && isZoomOut)
	{
		Vector3 out;
		const Vector3 pos = m_eyePos + dir * len;
		if (m_boundingHSphere.GetBoundingPoint(pos, out))
			return;
	}

	m_eyePos += dir * len;
	m_lookAt += dir * len;
	UpdateViewMatrix();
}


void cCamera::Move(const Vector3 &lookAt
	, const float velocity //= 0.f
)
{
	// Initialize mover vector
	m_mover.clear();

	const Vector3 dir = GetDirection();
	const float dist = m_eyePos.Distance(m_lookAt);
	const Vector3 eyePos = lookAt + (dir * -dist);

	sCamMoving info;
	info.eyePos = eyePos;
	info.lookAt = lookAt;

	//const float eyePosVelocity = (eyePos - m_eyePos).Length();
	//const float lookAtVelocity = (lookAt - m_lookAt).Length();
	info.velocityLookAt = velocity;
	info.velocityPos = velocity;

	m_mover.push_back(info);
	m_state = eState::MOVE;
}


void cCamera::Move(const Vector3 &eyePos, const Vector3 &lookAt
	, const float velocity //= 0.f
)
{
	// Initialize mover vector
	m_mover.clear();

	sCamMoving info;
	info.eyePos = eyePos;
	info.lookAt = lookAt;

	info.velocityLookAt = velocity;
	info.velocityPos = velocity;

	m_mover.push_back(info);
	m_state = eState::MOVE;
}


void cCamera::MoveNext(const Vector3 &eyePos, const Vector3 &lookAt
	, const float velocity //= 0.f
)
{
	sCamMoving info;
	info.eyePos = eyePos;
	info.lookAt = lookAt;

	Vector3 curEyePos = (m_mover.empty()) ? m_eyePos : m_mover.back().eyePos;
	Vector3 curLookAt = (m_mover.empty()) ? m_lookAt : m_mover.back().lookAt;

	info.velocityLookAt = velocity;
	info.velocityPos = velocity;

	m_mover.push_back(info);
	m_state = eState::MOVE;
}


Matrix44 cCamera::GetZoomMatrix() const 
{ 
	return Matrix44::Identity; 
}


float cCamera::GetZoom() const
{
	return 1.f;
}


// Right 축으로 회전한다.
void cCamera::Pitch(const float radian)
{
	RET(radian == 0);

	const Vector3 axis = GetRight();
	const Quaternion q(axis, radian);
	const Matrix44 mat = q.GetMatrix();

	Vector3 v = m_lookAt - m_eyePos;
	v *= mat;
	Vector3 lookAt = m_eyePos + v;

	const Vector3 dir = (lookAt - m_eyePos).Normal();
	if (abs(dir.DotProduct(Vector3(0,1,0))) > 0.95f)
		return;

	m_lookAt = m_eyePos + v;

	UpdateViewMatrix();
}


// Up 축으로 회전한다.
void cCamera::Yaw(const float radian)
{
	RET(radian == 0);

	const Vector3 axis = GetUpVector();
	const Quaternion q(axis, radian);
	const Matrix44 mat = q.GetMatrix();

	Vector3 v = m_lookAt - m_eyePos;
	v *= mat;
	m_lookAt = m_eyePos + v;

	UpdateViewMatrix();
}


// Direction 축으로 회전한다.
void cCamera::Roll(const float radian)
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
