//
// 2017-07-27, jjuiddong
// Dx11 Shader class
//
#pragma once


namespace graphic
{
	using namespace common;

	class cShader11
	{
	public:
		cShader11();
		virtual ~cShader11();

		bool Create(cRenderer &renderer, const StrPath &fileName
			, const char *techniqueName , const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements);
		bool Create(cRenderer &renderer, const StrPath &fileName
			, const char *techniqueName, const int vtxType);
		bool CreateFromFile(cRenderer &renderer, const StrPath &fileName
			, const char *techniqueName, const int vtxType);
		int Begin();
		void BeginPass(cRenderer &renderer, const int pass);
		bool SetTechnique(const char *id);
		bool CompileAndReload(cRenderer &renderer);
		bool OpenFile(cRenderer &renderer, const char *operation = "edit") const;
		void Clear();

		ID3DX11EffectTechnique* GetTechnique(const char *id);
		ID3DX11EffectVariable* GetVariable(const char *id);
		ID3DX11EffectMatrixVariable* GetMatrix(const char *id);
		ID3DX11EffectVectorVariable* GetVector(const char *id);

		static bool Compile(const char *fileName, Str512 *outMsg = NULL );


	public:
		StrId m_name;
		StrPath m_fxoFileName; // *.fxo fileName
		ID3DX11Effect *m_effect;
		ID3DX11EffectTechnique* m_technique;
		cVertexLayout m_vtxLayout;
	};

}
