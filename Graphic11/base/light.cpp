
#include "stdafx.h"
#include "light.h"

using namespace graphic;

cLight::cLight()
{
}

cLight::~cLight()
{
}


void cLight::Init(TYPE type, 
	const Vector4 &ambient, // Vector4(0.3f, 0.3f, 0.3f, 1),
	const Vector4 &diffuse, // Vector4(0.9f, 0.9f, 0.9f,1),
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
	return cb;
}
