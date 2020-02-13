
#include "stdafx.h"
#include "light.h"

using namespace graphic;

cLight::cLight()
	: m_specExp(0.5f)
	, m_specIntensity(1.f)
{
}

cLight::~cLight()
{
}


void cLight::Init(const TYPE type, 
	const Vector4 &ambient, // Vector4(1.f, 1.f, 1.f, 1.f),
	const Vector4 &diffuse, // Vector4(1.f, 1.f, 1.f, 1.f),
	const Vector4 &specular, // Vector4(0.2,0.2,0.2,0.2)
	const Vector3 &direction) // Vector3(0,-1,0)
{
	m_ambient = ambient;
	m_diffuse = diffuse;
	m_specular = specular;
	m_direction = direction;
	m_pos = Vector3(0, 0, 0);
}


// set position, direction
void cLight::Set(const cCamera &camera)
{
	const Vector3 lightPos = camera.GetEyePos();
	const Vector3 lightLookat = camera.GetLookAt();
	SetPosition(lightPos);
	SetDirection((lightLookat - lightPos).Normal());
}


void cLight::SetDirection( const Vector3 &direction )
{
	m_direction = direction;
}


void cLight::SetPosition( const Vector3 &pos )
{
	m_pos = pos;
}


void cLight::Bind(cRenderer &renderer) 
{
	renderer.m_cbLight = GetLight();
}


sCbLight cLight::GetLight()
{
	sCbLight cb;
	cb.ambient = XMLoadFloat4((XMFLOAT4*)&m_ambient);
	cb.diffuse = XMLoadFloat4((XMFLOAT4*)&m_diffuse);
	cb.specular = XMLoadFloat4((XMFLOAT4*)&m_specular);
	cb.direction = XMLoadFloat3((XMFLOAT3*)&m_direction);
	cb.posW = XMLoadFloat3((XMFLOAT3*)&m_pos);
	cb.specIntensity = XMLoadFloat3((XMFLOAT3*)&Vector3(m_specExp, m_specIntensity, 0));
	return cb;
}


// 광원 위치와 방향을 이용해, 카메라 행렬을 만들어 리턴한다.
// pair<Mat,Mat> : View Matrix, Projection Matrix
std::pair<Matrix44, Matrix44> cLight::GetCameraMatrix(const float fov
	, const float aspect, const float nearPlane, const float farPlane)
{
	Matrix44 view;
	view.SetView(m_pos, m_direction, Vector3(0,1,0));

	Matrix44 proj;
	proj.SetProjection(fov, aspect, nearPlane, farPlane);

	return{ view, proj };
}


// 광원 위치와 방향을 이용해, 카메라 행렬을 만들어 리턴한다.
// tuple<Mat,Mat,Mat> : View Matrix
//						Projection Matrix
//						transform Texture UV Coords Matrix
std::tuple<Matrix44, Matrix44, Matrix44> cLight::GetShadowMatrix(const float fov
	, const float aspect, const float nearPlane, const float farPlane)
{
	Matrix44 view;
	view.SetView(m_pos, m_direction, Vector3(0, 1, 0));

	Matrix44 proj;
	proj.SetProjection(fov, aspect, nearPlane, farPlane);

	Matrix44 mTT(0.5f, 0.0f, 0.0f, 0.0f
		, 0.0f, -0.5f, 0.0f, 0.0f
		, 0.0f, 0.0f, 1.0f, 0.0f
		, 0.5f, 0.5f, 0.0f, 1.0f);

	return std::make_tuple(view, proj, mTT);
}
