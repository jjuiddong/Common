//
// 2017-05-08, jjuiddong
// shader macro define
//
#pragma once


#define SHADER_SETFLOAT_MACRO(funcname, key, handle)\
	D3DXHANDLE handle=NULL;\
	void funcname(const float &val)\
	{\
		RET(!m_effect);\
		if (!handle)\
			handle = m_effect->GetParameterByName(NULL, key);\
		if (FAILED(m_effect->SetFloat(handle, val)))\
		{\
			assert(0);\
			MessageBoxA(NULL, \
				format("cShader::"#funcname" [%s] Error", key).c_str(), "ERROR", MB_OK);\
		}\
	}


#define SHADER_SETVECTOR_MACRO(funcname, key, handle)\
	D3DXHANDLE handle=NULL;\
	void funcname(const Vector3 &vec)\
	{\
		RET(!m_effect);\
		if (!handle)\
			handle = m_effect->GetParameterByName(NULL, key);\
		if (FAILED(m_effect->SetVector(handle, &D3DXVECTOR4(vec.x, vec.y, vec.z, 1))))\
		{\
			assert(0);\
			MessageBoxA(NULL, \
				format("cShader::"#funcname" [%s] Error", key).c_str(), "ERROR", MB_OK);\
		}\
	}

#define SHADER_SETMATRIX_MACRO(funcname, key, handle)\
	D3DXHANDLE handle=NULL;\
	void funcname(const Matrix44 &mat)\
	{\
		RET(!m_effect);\
		if (!handle)\
			handle = m_effect->GetParameterByName(NULL, key);\
		if (FAILED(m_effect->SetMatrix(handle, (D3DXMATRIX*)&mat)))\
		{\
			assert(0);\
			MessageBoxA(NULL, \
				format("cShader::"#funcname" [%s] Error", key).c_str(), "ERROR", MB_OK);\
		}\
	}

