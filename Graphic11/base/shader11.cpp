
#include "stdafx.h"
#include "shader11.h"


using namespace graphic;

cShader11::cShader11()
: m_vtxShader(NULL)
, m_pixelShader(NULL)
{
}

cShader11::~cShader11()
{
	SAFE_RELEASE(m_vtxShader);
	SAFE_RELEASE(m_pixelShader);
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(const char* szFileName, const char *szEntryPoint
	, const char *szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFileA(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}


bool cShader11::CreateVertexShader(cRenderer &renderer, const StrPath &fileName, const char *entryPoint
	, const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements)
{
	ID3DBlob* pVSBlob = NULL;
	if (FAILED(CompileShaderFromFile(fileName.c_str(), entryPoint, "vs_4_0", &pVSBlob)))
	{
		return false;
	}

	if (FAILED(renderer.GetDevice()->CreateVertexShader(pVSBlob->GetBufferPointer()
		, pVSBlob->GetBufferSize(), NULL, &m_vtxShader)))
	{
		pVSBlob->Release();
		return false;
	}

	if (!m_vtxLayout.Create(renderer, pVSBlob, layout, numElements))
	{
		pVSBlob->Release();
		return false;
	}

	pVSBlob->Release();

	return true;
}


bool cShader11::CreatePixelShader(cRenderer &renderer, const StrPath &fileName, const char *entryPoint)
{
	ID3DBlob* pPSBlob = NULL;
	if (FAILED(CompileShaderFromFile(fileName.c_str(), entryPoint, "ps_4_0", &pPSBlob)))
	{
		return false;
	}

	// Create the pixel shader
	if (FAILED(renderer.GetDevice()->CreatePixelShader(pPSBlob->GetBufferPointer()
		, pPSBlob->GetBufferSize(), NULL, &m_pixelShader)))
	{
		pPSBlob->Release();
		return false;
	}

	pPSBlob->Release();

	return true;
}


void cShader11::BindVertexShader(cRenderer &renderer)
{
	m_vtxLayout.Bind(renderer);
	renderer.GetDevContext()->VSSetShader(m_vtxShader, NULL, 0);
}


void cShader11::BindPixelShader(cRenderer &renderer)
{
	renderer.GetDevContext()->PSSetShader(m_pixelShader, NULL, 0);
}
