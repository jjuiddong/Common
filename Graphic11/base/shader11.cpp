
#include "stdafx.h"
#include "shader11.h"


using namespace graphic;

cShader11::cShader11()
	: m_technique(NULL)
{
}

cShader11::~cShader11()
{
	//SAFE_RELEASE(m_vtxShader);
	//SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_effect);
}


bool cShader11::Create(cRenderer &renderer, const StrPath &fileName
	, const char *techniqueName, const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements)
{
	std::ifstream fin(fileName.c_str(), std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HRESULT hr = D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, renderer.GetDevice(), &m_effect);
	if (FAILED(hr))
		return false;

	m_technique = m_effect->GetTechniqueByName(techniqueName);
	RETV(!m_technique, false);

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	m_technique->GetPassByIndex(0)->GetDesc(&passDesc);

	if (!m_vtxLayout.Create(renderer, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, layout, numElements))
		return false;

	m_name = fileName.GetFileName();

	return true;
}


bool cShader11::SetTechnique(const char *id)
{
	RETV(!m_effect, false);

	ID3DX11EffectTechnique *tech = m_effect->GetTechniqueByName(id);
	RETV(!tech, false);
	m_technique = tech;
	return true;
}


ID3DX11EffectTechnique* cShader11::GetTechnique(const char *id)
{
	return NULL;
}


ID3DX11EffectVariable* cShader11::GetVariable(const char *id)
{
	return NULL;
}


ID3DX11EffectMatrixVariable* cShader11::GetMatrix(const char *id)
{
	//return mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	return NULL;
}


ID3DX11EffectVectorVariable** cShader11::GetVector(const char *id)
{
	return NULL;
}


int cShader11::Begin()
{
	RETV(!m_technique, 0);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_technique->GetDesc(&techDesc);
	return techDesc.Passes;
}


void cShader11::BeginPass(cRenderer &renderer, const int pass)
{
	RET(!m_technique);
	m_vtxLayout.Bind(renderer);
	m_technique->GetPassByIndex(pass)->Apply(0, renderer.GetDevContext());

	for (int i = 0; i < cRenderer::MAX_TEXTURE_STAGE; ++i)
	{
		if (!renderer.m_textureMap[i])
			break;
		renderer.GetDevContext()->PSSetShaderResources(i + 2, 1, &renderer.m_textureMap[i]);
	}
}
