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

		bool CreateVertexShader(cRenderer &renderer, const StrPath &fileName, const char *entryPoint
			, const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements);
		bool CreatePixelShader(cRenderer &renderer, const StrPath &fileName, const char *entryPoint);
		void BindVertexShader(cRenderer &renderer);
		void BindPixelShader(cRenderer &renderer);


	public:
		ID3D11VertexShader *m_vtxShader;
		ID3D11PixelShader *m_pixelShader;
		cVertexDeclaration m_vtxDecl;
	};

}
