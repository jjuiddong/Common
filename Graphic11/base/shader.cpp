
#include "stdafx.h"
#include "shader.h"

using namespace graphic;


cShader::cShader() :
	m_effect(NULL) 
,	m_hTechnique(NULL)
,	m_renderPass(0)
, m_isShowMsgBox(false)
, m_lightDir(NULL)
, m_lightPos(NULL)
, m_lightAmbient(NULL)
, m_lightDiffuse(NULL)
, m_lightSpecular(NULL)
, m_lightTheta(NULL)
, m_lightPhi(NULL)
, m_cameraView(NULL)
, m_cameraProj(NULL)
, m_cameraViewProj(NULL)
, m_cameraEyePos(NULL)
, m_mtrlAmbient(NULL)
, m_mtrlDiffuse(NULL)
, m_mtrlEmissive(NULL)
, m_mtrlSpecular(NULL)
, m_mtrlShininess(NULL)
{
}


cShader::~cShader()
{
	Clear();
}


bool cShader::Create(cRenderer &renderer, const StrPath &fileName
	, const Str32 &technique
	, const bool showMsgBox//=true
)
{
	Clear();

	//HRESULT hr;
	//LPD3DXBUFFER pErr;
	//if (FAILED(hr = D3DXCreateEffectFromFileA(renderer.GetDevice(), fileName.c_str(), NULL,
	//	NULL, D3DXSHADER_DEBUG, NULL, &m_effect, &pErr))) 
	//{
	//	if (pErr)
	//	{
	//		dbg::ErrLog("cShader::Create Error pErr!=NULL\n");
	//		MessageBoxA( NULL, (LPCSTR)pErr->GetBufferPointer(), "ERROR", MB_OK);
	//	}
	//	else
	//	{
 //			dbg::ErrLog("cShader::Create Error pErr==NULL [%s], device=%x \n", 
 //				fileName.c_str(), renderer.GetDevice());
	//		StrPath msg = fileName + " Not Exist File";
	//		if (showMsgBox)
	//			MessageBoxA( NULL, msg.c_str(), "ERROR", MB_OK);
	//	}

	//	//DXTRACE_ERR( "CreateEffectFromFile", hr );
	//	return false;
	//}

	m_fileName = fileName;
	m_isShowMsgBox = showMsgBox;
	SetTechnique(technique);

	m_lightDir = NULL;
	m_lightPos = NULL;
	m_lightAmbient = NULL;
	m_lightDiffuse = NULL;
	m_lightSpecular = NULL;
	m_lightTheta = NULL;
	m_lightPhi = NULL;

	m_cameraView = NULL;
	m_cameraProj = NULL;
	m_cameraViewProj = NULL;
	m_cameraEyePos = NULL;

	m_mtrlAmbient = NULL;
	m_mtrlDiffuse = NULL;
	m_mtrlEmissive = NULL;
	m_mtrlSpecular = NULL;
	m_mtrlShininess = NULL;

	return true;
}


bool cShader::Reload(cRenderer &renderer)
{
	return Create(renderer, m_fileName, m_technique, m_isShowMsgBox);
}


void cShader::SetTechnique(const Str32 &technique) 
{
	RET(!m_effect);
	m_hTechnique = m_effect->GetTechniqueByName(technique.c_str());
	m_technique = technique;
}


int cShader::Begin()
{
	RETV(!m_effect, 0);
	u_int passCount = 0;
	m_effect->SetTechnique(m_hTechnique);
	m_effect->Begin(&passCount, 0);
	return passCount;
}


void cShader::BeginPass(int pass) // pass=-1
{
	RET(!m_effect);
	m_effect->BeginPass( (pass == -1)? m_renderPass : pass );
}


void cShader::EndPass()
{
	RET(!m_effect);
	m_effect->EndPass();
}


void cShader::End()
{
	RET(!m_effect);
	m_effect->End();
}


void cShader::SetBool(const Str32 &key, const bool val)
{
	RET(!m_effect);
	if (FAILED(m_effect->SetBool(key.c_str(), val)))
	{
		assert(0);
		MessageBoxA(NULL, "cShader::SetBool Error", "ERROR", MB_OK);
	}
}

void cShader::SetInt(const Str32 &key, const int val )
{
	RET(!m_effect);
	if (FAILED(m_effect->SetInt( key.c_str(), val)))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetInt Error", "ERROR", MB_OK);
	}	
}

void cShader::SetMatrix(const Str32 &key, const Matrix44 &mat)
{
	RET(!m_effect);
	if (FAILED(m_effect->SetMatrix( key.c_str(), (D3DXMATRIX*)&mat)))
	{
		assert(0);
		MessageBoxA(NULL, format("cShader::SetMatrix [%s] Error", key.c_str()).c_str(), "ERROR", MB_OK);
	}
}


void cShader::SetTexture(const Str32 &key, cTexture &texture)
{
	RET(!m_effect);
	if (FAILED(m_effect->SetTexture( key.c_str(), texture.GetTexture())))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetTexture Error", "ERROR", MB_OK);
	}
}
void cShader::SetCubeTexture(const Str32 &key, cCubeTexture &texture)
{
	RET(!m_effect);
	if (FAILED(m_effect->SetTexture(key.c_str(), texture.m_texture)))
	{
		assert(0);
		MessageBoxA(NULL, "cShader::SetCubeTexture Error", "ERROR", MB_OK);
	}
}
void cShader::SetTexture(const Str32 &key, IDirect3DTexture9 *texture)
{
	RET(!m_effect);
	if (FAILED(m_effect->SetTexture( key.c_str(), texture)))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetTexture Error", "ERROR", MB_OK);
	}
}

void cShader::SetTexture(const Str32 &key, IDirect3DCubeTexture9 *texture)
{
	RET(!m_effect);
	if (FAILED(m_effect->SetTexture( key.c_str(), texture)))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetTexture Error", "ERROR", MB_OK);
	}
}

void cShader::SetFloat(const Str32 &key, float val)
{
	RET(!m_effect);
	if (FAILED(m_effect->SetFloat( key.c_str(), val)))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetFloat Error", "ERROR", MB_OK);
	}	
}


void cShader::SetVector(const Str32 &key, const Vector2 &vec )
{
	RET(!m_effect);
	if (FAILED(m_effect->SetVector( key.c_str(), &D3DXVECTOR4(vec.x, vec.y, 0, 1.f))))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetVector Error", "ERROR", MB_OK);
	}	
}


void cShader::SetVector(const Str32 &key, const Vector3 &vec )
{
	RET(!m_effect);
	if (FAILED(m_effect->SetVector( key.c_str(), &D3DXVECTOR4(vec.x, vec.y, vec.z, 1.f))))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetVector Error", "ERROR", MB_OK);
	}	
}


void cShader::SetVector(const Str32 &key, const Vector4 &vec )
{
	RET(!m_effect);
	if (FAILED(m_effect->SetVector( key.c_str(), &D3DXVECTOR4(vec.x, vec.y, vec.z, vec.w))))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetVector Error", "ERROR", MB_OK);
	}	
}


void cShader::SetMatrixArray(const Str32 &key, const Matrix44 *mat, const int count )
{
	RET(!m_effect);
	if (FAILED(m_effect->SetMatrixArray(key.c_str(), (D3DXMATRIX*)mat, count)))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetMatrixArray Error", "ERROR", MB_OK);
	}
}


void cShader::SetIntArray(const Str32 &key, const int *array, const int count)
{
	RET(!m_effect);
	if (FAILED(m_effect->SetIntArray(key.c_str(), array, count)))
	{
		assert(0);
		MessageBoxA(NULL, "cShader::SetIntArray Error", "ERROR", MB_OK);
	}
}


void cShader::SetBool(D3DXHANDLE handle, const bool val)
{
	RET(!m_effect);
	if (FAILED(m_effect->SetBool(handle, val)))
	{
		assert(0);
		MessageBoxA(NULL, "cShader::SetInt Error", "ERROR", MB_OK);
	}
}

void cShader::SetInt(D3DXHANDLE handle, const int val )
{
	RET(!m_effect);
	if (FAILED(m_effect->SetInt(handle, val)))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetInt Error", "ERROR", MB_OK);
	}	
}

void cShader::SetMatrix(D3DXHANDLE handle, const Matrix44 &mat)
{
	RET(!m_effect);
	if (FAILED(m_effect->SetMatrix(handle, (D3DXMATRIX*)&mat)))
	{
		assert(0);
		MessageBoxA( NULL, format("cShader::SetMatrix [%s] Error", (char*)handle).c_str(), "ERROR", MB_OK);
	}
}


void cShader::SetTexture(D3DXHANDLE handle, cTexture &texture)
{
	RET(!m_effect);
	if (FAILED(m_effect->SetTexture(handle, texture.GetTexture())))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetTexture Error", "ERROR", MB_OK);
	}
}
void cShader::SetTexture(D3DXHANDLE handle, IDirect3DTexture9 *texture)
{
	RET(!m_effect);
	if (FAILED(m_effect->SetTexture(handle, texture)))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetTexture Error", "ERROR", MB_OK);
	}
}

void cShader::SetFloat(D3DXHANDLE handle, float val)
{
	RET(!m_effect);
	if (FAILED(m_effect->SetFloat(handle, val)))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetFloat Error", "ERROR", MB_OK);
	}	
}


void cShader::SetVector(D3DXHANDLE handle, const Vector2 &vec )
{
	RET(!m_effect);
	if (FAILED(m_effect->SetVector(handle, &D3DXVECTOR4(vec.x, vec.y, 0, 1.f))))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetVector Error", "ERROR", MB_OK);
	}	
}


void cShader::SetVector(D3DXHANDLE handle, const Vector3 &vec )
{
	RET(!m_effect);
	if (FAILED(m_effect->SetVector(handle, &D3DXVECTOR4(vec.x, vec.y, vec.z, 1.f))))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetVector Error", "ERROR", MB_OK);
	}	
}


void cShader::SetVector(D3DXHANDLE handle, const Vector4 &vec )
{
	RET(!m_effect);
	if (FAILED(m_effect->SetVector(handle, &D3DXVECTOR4(vec.x, vec.y, vec.z, vec.w))))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetVector Error", "ERROR", MB_OK);
	}	
}


void cShader::SetMatrixArray(D3DXHANDLE handle, const Matrix44 *mat, const int count )
{
	RET(!m_effect);
	if (FAILED(m_effect->SetMatrixArray(handle, (D3DXMATRIX*)mat, count)))
	{
		assert(0);
		MessageBoxA( NULL, "cShader::SetMatrixArray Error", "ERROR", MB_OK);
	}
}


void cShader::SetIntArray(D3DXHANDLE handle, const int *array, const int count)
{
	RET(!m_effect);
	if (FAILED(m_effect->SetIntArray(handle, array, count)))
	{
		assert(0);
		MessageBoxA(NULL, "cShader::SetIntArray Error", "ERROR", MB_OK);
	}
}


void cShader::CommitChanges()
{
	RET(!m_effect);
	m_effect->CommitChanges();
}


D3DXHANDLE cShader::GetValueHandle(const Str32 &key)
{
	RETV(!m_effect, NULL);
	return m_effect->GetParameterByName(NULL, key.c_str());
}


void cShader::LostDevice()
{
	//SAFE_RELEASE(m_effect);
	m_effect->OnLostDevice();
}


void cShader::ResetDevice(cRenderer &renderer)
{
	RET(m_fileName.empty());
	m_effect->OnResetDevice();
	//Create(renderer, m_fileName, m_technique, m_isShowMsgBox);
}


void cShader::Clear()
{
	SAFE_RELEASE(m_effect);
}
