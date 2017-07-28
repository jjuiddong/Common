
#include "stdafx.h"
#include "dxinit.h"

using namespace graphic;


bool graphic::InitDirectX(HWND hWnd, const int width, const int height, const UINT adapter, 
	OUT D3DPRESENT_PARAMETERS &param, OUT LPDIRECT3DDEVICE9 &pDevice )
{
	LPDIRECT3D9 d3d9;
	d3d9 = Direct3DCreate9( D3D_SDK_VERSION );

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);

	int vertexProcessing = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS d3dpp;		
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferHeight = height;
	d3dpp.BackBufferWidth = width;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_8_SAMPLES;
	//d3dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
	//d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = true;

	if (FAILED(d3d9->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		vertexProcessing 
		| D3DCREATE_MULTITHREADED
		,
		&d3dpp,
		&pDevice
		)))
	{
		d3d9->Release();
		d3d9 = NULL;
		MessageBoxA( hWnd, "CreateDevice() - FAILED", "FAILED", MB_OK );
		return false;
	}

	d3d9->Release();
	d3d9 = NULL;
	param = d3dpp;
	return true;
}


bool graphic::InitDirectX11(HWND hWnd, const int width, const int height, const UINT adapter
	, OUT ID3D11Device **pd3dDevice
	, OUT ID3D11DeviceContext **pImmediateContext
	, OUT IDXGISwapChain **pSwapChain 
	, OUT ID3D11RenderTargetView **pRenderTargetView
)
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, pSwapChain, pd3dDevice, &featureLevel, pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return false;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = (*pSwapChain)->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return false;

	hr = (*pd3dDevice)->CreateRenderTargetView(pBackBuffer, NULL, pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return false;

	(*pImmediateContext)->OMSetRenderTargets(1, pRenderTargetView, NULL);

	return true;
}
