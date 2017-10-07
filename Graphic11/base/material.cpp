
#include "stdafx.h"
#include "material.h"

using namespace graphic;


cMaterial::cMaterial()
{
	InitRed();
}

cMaterial::~cMaterial()
{
}


// 메터리얼 초기화.
void cMaterial::Init(
	const Vector4 &ambient 
	, const Vector4 &diffuse
	, const Vector4 &specular
	, const Vector4 &emmisive //=Vector4(0, 0, 0, 1),
	, const float pow // =90
)
{
	m_ambient = ambient;
	m_diffuse = diffuse;
	m_specular = specular;
	m_emissive = emmisive;
	m_power = pow;
}



void cMaterial::Init(const sMaterial &mtrl)
{
	Init(mtrl.ambient, mtrl.diffuse, mtrl.specular, mtrl.emissive);
	m_power = mtrl.power;
}


void cMaterial::InitWhite()
{
	//Init(Vector4(1,1,1,1), Vector4(1,1,1,1), Vector4(1,1,1,1));
	Init(Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(1, 1, 1, 1), Vector4(1, 1, 1, 1));
}


void cMaterial::InitBlack()
{
	Init(Vector4(0,0,0,1), Vector4(0,0,0,1), Vector4(0,0,0,1));
}


void cMaterial::InitGray()
{
	//Init(Vector4(0.5f, 0.5f, 0.5f, 1), Vector4(0.5f, 0.5f, 0.5f, 1), Vector4(0, 0, 0, 1));
	Init(Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.5f, 0.5f, 0.5f, 1), Vector4(0, 0, 0, 1));
}


void cMaterial::InitGray2()
{
	//Init(Vector4(0.9f, 0.9f, 0.9f, 1), Vector4(0.9f, 0.9f, 0.9f, 1), Vector4(0, 0, 0, 1));
	Init(Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.9f, 0.9f, 0.9f, 1), Vector4(0, 0, 0, 1));
}

void cMaterial::InitGray3()
{
	//Init(Vector4(0.3f, 0.3f, 0.3f, 1), Vector4(0.7f, 0.7f, 0.7f, 1), Vector4(1, 1, 1, 1));
	Init(Vector4(0.2f, 0.2f, 0.2f, 1), Vector4(0.7f, 0.7f, 0.7f, 1), Vector4(1, 1, 1, 1));
}

void cMaterial::InitXFile()
{
	Init(Vector4(0.f, 0.f, 0.f, 1), Vector4(1.f, 1.f, 1.f, 1), Vector4(1, 1, 1, 1));
}

void cMaterial::InitRed()
{
	//Init(Vector4(1,0,0,1), Vector4(1,0,0,1), Vector4(1,0,0,1));
	Init(Vector4(0.2f, 0, 0, 1), Vector4(1, 0, 0, 1), Vector4(1, 0, 0, 1));
}


void cMaterial::InitBlue()
{
	//Init(Vector4(0,0,1,1), Vector4(0,0,1,1), Vector4(0,0,1,1));
	Init(Vector4(0, 0, 0.2f, 1), Vector4(0, 0, 1, 1), Vector4(0, 0, 1, 1));
}


void cMaterial::InitGreen()
{
	//Init(Vector4(0, 1, 0, 1), Vector4(0, 1, 0, 1), Vector4(0, 1, 0, 1));
	Init(Vector4(0, 0.2f, 0, 1), Vector4(0, 1, 0, 1), Vector4(0, 1, 0, 1));
}


void cMaterial::InitYellow()
{
	//Init(Vector4(1, 1, 0, 1), Vector4(1, 1, 0, 1), Vector4(1, 1, 0, 1));
	Init(Vector4(0.2f, 0.2f, 0, 1), Vector4(1, 1, 0, 1), Vector4(1, 1, 0, 1));
}


Str64 cMaterial::DiffuseColor()
{
	Str64 str = common::format<64>("Diffuse  R=%f G=%f B=%f A=%f ",
		m_diffuse.x, m_diffuse.y, m_diffuse.z, m_diffuse.w);

	Str64 color = SpecialColor(m_diffuse.x, m_diffuse.y, m_diffuse.z);

	if (!color.empty())
		str += Str64(" (") + color + ")";
	return str;
}

Str64 cMaterial::AmbientColor()
{
	Str64 str = common::format<64>("Ambient  R=%f G=%f B=%f A=%f ",
		m_ambient.x, m_ambient.y, m_ambient.z, m_ambient.w);

	Str64 color = SpecialColor(m_ambient.x, m_ambient.y, m_ambient.z);

	if (!color.empty())
		str += Str64(" (") + color + ")";
	return str;
}

Str64 cMaterial::SpecularColor()
{
	Str64 str = common::format<64>("Specular  R=%f G=%f B=%f A=%f ",
		m_specular.x, m_specular.y, m_specular.z, m_specular.w);

	Str64 color = SpecialColor(m_specular.x, m_specular.y, m_specular.z);

	if (!color.empty())
		str += Str64(" (") + color + ")";

	return str;
}

Str64 cMaterial::EmissiveColor()
{
	Str64 str = common::format<64>("Emissive  R=%f G=%f B=%f A=%f ",
		m_emissive.x, m_emissive.y, m_emissive.z, m_emissive.w);

	Str64 color = SpecialColor(m_emissive.x, m_emissive.y, m_emissive.z);

	if (!color.empty())
		str += Str64(" (") + color + ")";

	return str;
}


// return Special Color Name
// r,g,b = 0 ~ 1
Str64 cMaterial::SpecialColor(const float r, const float g, const float b)
{
	Str64 color;
	if ((r == 1) && (g == 1) && (b == 1))
		color = "White";
	else if ((r == 0) && (g == 0) && (b == 0))
		color = "Black";
	else if ((r == 1) && (g == 0) && (b == 0))
		color = "Red";
	else if ((r == 0) && (g == 1) && (b == 0))
		color = "Green";
	else if ((r == 0) && (g == 0) && (b == 1))
		color = "Blue";
	return color;
}


sCbMaterial cMaterial::GetMaterial()
{
	sCbMaterial cb;
	cb.ambient = XMLoadFloat4((XMFLOAT4*)&m_ambient);
	cb.diffuse = XMLoadFloat4((XMFLOAT4*)&m_diffuse);
	cb.specular = XMLoadFloat4((XMFLOAT4*)&m_specular);
	cb.emissive = XMLoadFloat4((XMFLOAT4*)&m_emissive);
	cb.pow = m_power;
	return cb;
}
