//
// 2017-05-03, jjuiddong
// 쉐이더 클래스
// LPD3DXEFFECT 인터페이스를 쉽게사용하기 위해 만들어짐.
//
#pragma once


namespace graphic
{

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



	class cShader
	{
	public:
		cShader();
		virtual ~cShader();

		bool Create(cRenderer &renderer, const string &fileName, 
			const string &technique, const bool showMsgBox = true );
		bool Reload(cRenderer &renderer);
		int Begin();
		void BeginPass(int pass=-1);
		void EndPass();
		void End();
		void SetTechnique(const string &technique);

		void SetBool(const string &key, const bool val);
		void SetInt(const string &key, const int val );
		void SetMatrix(const string &key, const Matrix44 &mat);
		void SetTexture(const string &key, cTexture &texture);
		void SetTexture(const string &key, IDirect3DTexture9 *texture);
		void SetTexture(const string &key, IDirect3DCubeTexture9 *texture);
		void SetFloat(const string &key, float val);
		void SetVector(const string &key, const Vector2 &vec );
		void SetVector(const string &key, const Vector3 &vec );
		void SetVector(const string &key, const Vector4 &vec );
		void SetMatrixArray(const string &key, const Matrix44 *mat, const int count );
		void SetIntArray(const string &key, const int *array, const int count);

		void SetBool(D3DXHANDLE handle, const bool val);
		void SetInt(D3DXHANDLE handle, const int val );
		void SetMatrix(D3DXHANDLE handle, const Matrix44 &mat);
		void SetTexture(D3DXHANDLE handle, cTexture &texture);
		void SetTexture(D3DXHANDLE handle, IDirect3DTexture9 *texture);
		void SetFloat(D3DXHANDLE handle, float val);
		void SetVector(D3DXHANDLE handle, const Vector2 &vec );
		void SetVector(D3DXHANDLE handle, const Vector3 &vec );
		void SetVector(D3DXHANDLE handle, const Vector4 &vec );
		void SetMatrixArray(D3DXHANDLE handle, const Matrix44 *mat, const int count );
		void SetIntArray(D3DXHANDLE handle, const int *array, const int count);

		void CommitChanges();
		LPD3DXEFFECT GetEffect() { return m_effect; }
		void SetRenderPass(int pass);
		int GetRenderPass() const;
		D3DXHANDLE GetValueHandle(const string &key);
		const string& GetFileName() const;
		void LostDevice();
		void ResetDevice(cRenderer &renderer);
		void Clear();


		// Light
		SHADER_SETVECTOR_MACRO(SetLightDir, "g_light.dir", m_lightDir);
		SHADER_SETVECTOR_MACRO(SetLightPos, "g_light.pos", m_lightPos);
		SHADER_SETVECTOR_MACRO(SetLightAmbient, "g_light.ambient", m_lightAmbient);
		SHADER_SETVECTOR_MACRO(SetLightDiffuse, "g_light.diffuse", m_lightDiffuse);
		SHADER_SETVECTOR_MACRO(SetLightSpecular, "g_light.specular", m_lightSpecular);
		SHADER_SETFLOAT_MACRO(SetLightTheta, "g_light.spotInnerCone", m_lightTheta);
		SHADER_SETFLOAT_MACRO(SetLightPhi, "g_light.spotOuterCone", m_lightPhi);

		// Camera
		SHADER_SETMATRIX_MACRO(SetCameraView, "g_mView", m_cameraView);
		SHADER_SETMATRIX_MACRO(SetCameraProj, "g_mProj", m_cameraProj);
		SHADER_SETMATRIX_MACRO(SetCameraViewProj, "g_mVP", m_cameraViewProj);
		SHADER_SETVECTOR_MACRO(SetCameraEyePos, "g_vEyePos", m_cameraEyePos);

		// Material
		SHADER_SETVECTOR_MACRO(SetMaterialAmbient, "g_material.ambient", m_mtrlAmbient);
		SHADER_SETVECTOR_MACRO(SetMaterialDiffuse, "g_material.diffuse", m_mtrlDiffuse);
		SHADER_SETVECTOR_MACRO(SetMaterialEmissive, "g_material.emissive", m_mtrlEmissive);
		SHADER_SETVECTOR_MACRO(SetMaterialSpecular, "g_material.specular", m_mtrlSpecular);
		SHADER_SETFLOAT_MACRO(SetMaterialShininess, "g_material.shininess", m_mtrlShininess);


	public:
		LPD3DXEFFECT m_effect;
		D3DXHANDLE m_hTechnique;
		int m_renderPass; // default = 0;
		string m_fileName;
		string m_technique;
		bool m_isShowMsgBox;
	};


	inline void cShader::SetRenderPass(int pass) { m_renderPass = pass; }
	inline int cShader::GetRenderPass() const { return m_renderPass; }
	inline const string& cShader::GetFileName() const { return m_fileName; }
}
