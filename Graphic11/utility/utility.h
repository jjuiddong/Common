//
// 2017-07-28, jjuiddong
// Graphic Utility
//
#pragma once


namespace graphic
{

	const static XMMATRIX XMIdentity(
		1, 0, 0, 0
		, 0, 1, 0, 0
		, 0, 0, 1, 0
		, 0, 0, 0, 1);

	// DirectXTK
	size_t BitsPerPixel(_In_ DXGI_FORMAT fmt);

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	
	void createUAV(ID3D11Device* pd3dDevice, ID3D11Buffer * buf, UINT byte_width, UINT byte_stride,
		ID3D11UnorderedAccessView** ppUAV, ID3D11ShaderResourceView** ppSRV);

	void createTextureAndViews(ID3D11Device* pd3dDevice, UINT width, UINT height, DXGI_FORMAT format,
		ID3D11Texture2D** ppTex, ID3D11ShaderResourceView** ppSRV, ID3D11RenderTargetView** ppRTV);

}
