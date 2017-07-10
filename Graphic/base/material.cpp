
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
void cMaterial::Init(const Vector4 &ambient, 
	const Vector4 &diffuse,
	const Vector4 &specular,
	const Vector4 &emmisive,
	const float pow)
{
	ZeroMemory(&m_mtrl, sizeof(m_mtrl));
	m_mtrl.Ambient = *(D3DXCOLOR*)&ambient;
	m_mtrl.Diffuse = *(D3DXCOLOR*)&diffuse;
	m_mtrl.Specular = *(D3DXCOLOR*)&specular;
	m_mtrl.Emissive = *(D3DXCOLOR*)&emmisive;
	m_mtrl.Power = 200.f;
}


void cMaterial::Init(const D3DMATERIAL9 &mtrl)
{
	m_mtrl = mtrl;
}


void cMaterial::Init(const sMaterial &mtrl)
{
	Init(mtrl.ambient, mtrl.diffuse, mtrl.specular, mtrl.emissive);
	m_mtrl.Power = mtrl.power;
}


void cMaterial::InitWhite()
{
	Init(Vector4(1,1,1,1), Vector4(1,1,1,1), Vector4(1,1,1,1));
}


void cMaterial::InitBlack()
{
	Init(Vector4(0,0,0,1), Vector4(0,0,0,1), Vector4(0,0,0,1));
}


void cMaterial::InitGray()
{
	Init(Vector4(0.5f, 0.5f, 0.5f, 1), Vector4(0.5f, 0.5f, 0.5f, 1), Vector4(0, 0, 0, 1));
}


void cMaterial::InitGray2()
{
	Init(Vector4(0.9f, 0.9f, 0.9f, 1), Vector4(0.9f, 0.9f, 0.9f, 1), Vector4(0, 0, 0, 1));
}

void cMaterial::InitGray3()
{
	Init(Vector4(0.3f, 0.3f, 0.3f, 1), Vector4(0.7f, 0.7f, 0.7f, 1), Vector4(1, 1, 1, 1));
}

void cMaterial::InitXFile()
{
	Init(Vector4(0.f, 0.f, 0.f, 1), Vector4(1.f, 1.f, 1.f, 1), Vector4(1, 1, 1, 1));
}

void cMaterial::InitRed()
{
	Init(Vector4(1,0,0,1), Vector4(1,0,0,1), Vector4(1,0,0,1));
}


void cMaterial::InitBlue()
{
	Init(Vector4(0,0,1,1), Vector4(0,0,1,1), Vector4(0,0,1,1));
}


void cMaterial::InitGreen()
{
	Init(Vector4(0, 1, 0, 1), Vector4(0, 1, 0, 1), Vector4(0, 1, 0, 1));
}


void cMaterial::InitYellow()
{
	Init(Vector4(1, 1, 0, 1), Vector4(1, 1, 0, 1), Vector4(1, 1, 0, 1));
}

void cMaterial::Bind(cRenderer &renderer)
{
	renderer.GetDevice()->SetMaterial(&m_mtrl);
}

void cMaterial::Bind(cShader &shader)
{
	shader.SetMaterialAmbient(*(Vector4*)&m_mtrl.Ambient);
	shader.SetMaterialDiffuse(*(Vector4*)&m_mtrl.Diffuse);
	shader.SetMaterialEmissive(*(Vector4*)&m_mtrl.Emissive);
	shader.SetMaterialSpecular(*(Vector4*)&m_mtrl.Specular);
	shader.SetMaterialShininess(m_mtrl.Power);
}


Str64 cMaterial::DiffuseColor()
{
	Str64 str = common::format<64>("Diffuse  R=%f G=%f B=%f A=%f ",
		m_mtrl.Diffuse.r, m_mtrl.Diffuse.g, m_mtrl.Diffuse.b, m_mtrl.Diffuse.a);

	Str64 color = SpecialColor(m_mtrl.Diffuse.r, m_mtrl.Diffuse.g, m_mtrl.Diffuse.b);

	if (!color.empty())
		str += Str64(" (") + color + ")";
	return str;
}

Str64 cMaterial::AmbientColor()
{
	Str64 str = common::format<64>("Ambient  R=%f G=%f B=%f A=%f ",
		m_mtrl.Ambient.r, m_mtrl.Ambient.g, m_mtrl.Ambient.b, m_mtrl.Ambient.a);

	Str64 color = SpecialColor(m_mtrl.Ambient.r, m_mtrl.Ambient.g, m_mtrl.Ambient.b);

	if (!color.empty())
		str += Str64(" (") + color + ")";
	return str;
}

Str64 cMaterial::SpecularColor()
{
	Str64 str = common::format<64>("Specular  R=%f G=%f B=%f A=%f ",
		m_mtrl.Specular.r, m_mtrl.Specular.g, m_mtrl.Specular.b, m_mtrl.Specular.a);

	Str64 color = SpecialColor(m_mtrl.Specular.r, m_mtrl.Specular.g, m_mtrl.Specular.b);

	if (!color.empty())
		str += Str64(" (") + color + ")";

	return str;
}

Str64 cMaterial::EmissiveColor()
{
	Str64 str = common::format<64>("Emissive  R=%f G=%f B=%f A=%f ",
		m_mtrl.Emissive.r, m_mtrl.Emissive.g, m_mtrl.Emissive.b, m_mtrl.Emissive.a);

	Str64 color = SpecialColor(m_mtrl.Emissive.r, m_mtrl.Emissive.g, m_mtrl.Emissive.b);

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
