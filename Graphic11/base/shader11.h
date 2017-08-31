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
		bool SetTechnique(const char *id);


		ID3DX11EffectTechnique* GetTechnique(const char *id);
		ID3DX11EffectVariable* GetVariable(const char *id);
		ID3DX11EffectMatrixVariable* GetMatrix(const char *id);
		ID3DX11EffectVectorVariable** GetVector(const char *id);


	public:
		StrId m_name;
		ID3DX11Effect *m_effect;
		ID3DX11EffectTechnique* m_technique;
		ID3D11ShaderResourceView *m_shadowMap; // reference
		cVertexLayout m_vtxLayout;
	};

}
