//
// 2017-08-21, jjuiddong
// Shader Manager
//
#pragma once


namespace graphic
{

	class cShaderManager
	{
	public:
		cShaderManager();
		virtual ~cShaderManager();

		cShader11* LoadShader(cRenderer &renderer, const StrPath &fileName
			, const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements
			, const bool isVtxTypeHash = true);

		cShader11* LoadShader(cRenderer &renderer, const StrPath &fileName
			, const int vtxType, const bool isVtxTypeHash = true);

		cShader11* LoadShaderFromFile(cRenderer &renderer, const StrPath &fileName
			, const int vtxType, const bool isVtxTypeHash = true);

		cShader11* FindShader(const int vertexType);
		cShader11* FindShader(const StrPath &fileName);
		void SetShaderRootDirectory(const StrPath &path);
		const StrPath& GetShaderRootDirectory() const;
		void ReloadAll(cRenderer &renderer);
		void Clear();


	public:
		StrPath m_shaderRootPath; // default: C:
		map<hashcode, cShader11*> m_shaders; // key = fileName
		map<int, cShader11*> m_vtxTypeShaders; // key = vertex type
	};


	inline void cShaderManager::SetShaderRootDirectory(const StrPath &path) { m_shaderRootPath = path; }
	inline const StrPath& cShaderManager::GetShaderRootDirectory() const { return m_shaderRootPath; }
}
