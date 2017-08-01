#pragma once

namespace graphic
{

	// Direct3D9 Initialize
	bool InitDirectX(HWND hWnd, const int width, const int height, const UINT adapter,
		OUT D3DPRESENT_PARAMETERS &param, OUT LPDIRECT3DDEVICE9 &pDevice);


	// Direct3D11 Initialize
	bool InitDirectX11(HWND hWnd, const int width, const int height, const UINT adapter
		, OUT ID3D11Device **pd3dDevice, OUT ID3D11DeviceContext **pImmediateContext
		, OUT IDXGISwapChain **pSwapChain, OUT ID3D11RenderTargetView **pRenderTargetView
		, OUT ID3D11Texture2D **pDepthStencil, OUT ID3D11DepthStencilView **pDepthStencilView );

}
