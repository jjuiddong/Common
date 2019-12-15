
#include "stdafx.h"
#include "computeshader.h"

using namespace graphic;


cComputeShader::cComputeShader()
	: m_shader(nullptr)
{
}

cComputeShader::~cComputeShader()
{
	Clear();
}


bool cComputeShader::Create(cRenderer &renderer, const StrPath &fileName)
{
	Clear();

	// *.cso file read (HLSL compiler output file)
	std::ifstream fin(fileName.c_str(), std::ios::binary);
	if (!fin.is_open())
		return false;

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();
	//

	HRESULT hr = renderer.GetDevice()->CreateComputeShader(
		&compiledShader[0], size, nullptr, &m_shader);
	if (FAILED(hr))
		return false;

	return true;
}


void cComputeShader::Bind(cRenderer &renderer)
{
	renderer.GetDevContext()->CSSetShader(m_shader, nullptr, 0);
}


void cComputeShader::Unbind(cRenderer &renderer)
{
	renderer.GetDevContext()->CSSetShader(nullptr, nullptr, 0);
}


void cComputeShader::Clear()
{
	SAFE_RELEASE(m_shader);
}
