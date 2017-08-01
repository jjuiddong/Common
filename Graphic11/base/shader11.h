//
// 2017-07-27, jjuiddong
// Dx11 Shader class
//
#pragma once


namespace graphic
{

	class cShader11
	{
	public:
		cShader11();
		virtual ~cShader11();

		bool Create(cRenderer &renderer, const StrPath &fileName
			, const char *techniqueName , const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements);
		int Begin();
		void BeginPass(cRenderer &renderer, const int pass);


		ID3DX11EffectTechnique* GetTechnique(const char *id);
		ID3DX11EffectVariable* GetVariable(const char *id);
		ID3DX11EffectMatrixVariable* GetMatrix(const char *id);
		ID3DX11EffectVectorVariable** GetVector(const char *id);


	public:
		ID3DX11Effect *m_effect;
		ID3DX11EffectTechnique* m_technique;
		cVertexLayout m_vtxLayout;
	};

}
