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
		cShader11* FindShader(const int vertexType);
		cShader11* FindShader(const StrPath &fileName);
		void Clear();


	public:
		map<hashcode, cShader11*> m_shaders; // key = fileName
		map<int, cShader11*> m_vtxTypeShaders; // key = vertex type
	};

}
