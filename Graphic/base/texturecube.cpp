
#include "stdafx.h"
#include "texturecube.h"

using namespace graphic;


cCubeTexture::cCubeTexture()
	: m_texture(NULL)
{
}

cCubeTexture::~cCubeTexture()
{
	Clear();
}


bool cCubeTexture::Create(cRenderer &renderer, const string &fileName)
{
	Clear();

	m_fileName = fileName;

	if (FAILED(D3DXCreateCubeTextureFromFileA(renderer.GetDevice(),
		fileName.c_str(), &m_texture)))
	{
		return false;
	}

	return true;
}


void cCubeTexture::Bind(cShader &shader, const string &key)
{
	shader.SetCubeTexture(key, *this);
}


void cCubeTexture::Clear()
{
	SAFE_RELEASE(m_texture);
}


bool cCubeTexture::IsLoaded()
{
	return m_texture != NULL;
}


void cCubeTexture::LostDevice()
{
	RET(!m_texture);
	SAFE_RELEASE(m_texture);
}


void cCubeTexture::ResetDevice(cRenderer &renderer)
{
	SAFE_RELEASE(m_texture);

	if (!m_fileName.empty())
		Create(renderer, string(m_fileName));
}
