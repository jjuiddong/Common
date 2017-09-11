
#include "stdafx.h"
#include "shadermanager.h"


using namespace graphic;

cShaderManager::cShaderManager()
{
}

cShaderManager::~cShaderManager()
{
	Clear();
}


cShader11* cShaderManager::LoadShader(cRenderer &renderer, const StrPath &fileName
	, const D3D11_INPUT_ELEMENT_DESC layout[], const int numElements
	, const bool isVtxTypeHash //= true
)
{
	if (cShader11 *p = FindShader(fileName))
		return p;

	cShader11 *shader = NULL;
	const StrPath resourcePath = cResourceManager::Get()->GetResourceFilePath(fileName);
	if (resourcePath.empty())
		goto error;

	shader = new cShader11();
	if (!shader->Create(renderer, resourcePath, "Unlit", layout, numElements))
		goto error;

	if (shader)
	{
		m_shaders[ StrPath(fileName.GetFileName()).GetHashCode()] = shader;

		// Check Aready Exist
		if (isVtxTypeHash)
		{
			assert(m_vtxTypeShaders.end() == m_vtxTypeShaders.find(shader->m_vtxLayout.m_vertexType));
			m_vtxTypeShaders[shader->m_vtxLayout.m_vertexType] = shader;
		}
	}
	return shader;


error:
	StrPath msg = StrPath("Error LoadShader ") + fileName + " 파일이 존재하지 않습니다.";
	MessageBoxA(NULL, msg.c_str(), "ERROR", MB_OK);
	dbg::ErrLog("%s\n", msg.c_str());
	SAFE_DELETE(shader);
	return NULL;
}


cShader11* cShaderManager::FindShader(const int vertexType)
{
	auto it = m_vtxTypeShaders.find(vertexType);
	if (m_vtxTypeShaders.end() == it)
		return NULL; // not exist
	return it->second;
}


cShader11* cShaderManager::FindShader(const StrPath &fileName)
{
	auto it = m_shaders.find(StrPath(fileName.GetFileName()).GetHashCode());
	if (m_shaders.end() == it)
		return NULL; // not exist
	return it->second;
}


void cShaderManager::Clear() 
{
	for (auto &kv : m_shaders)
		SAFE_DELETE(kv.second);
	m_shaders.clear();
	m_vtxTypeShaders.clear();
}
