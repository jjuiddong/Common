
#include "stdafx.h"
#include "xfilemesh.h"
#include "xfilemodel.h"

using namespace graphic;



bool cXFileModel::Create(cRenderer &renderer, const string &fileName, const bool isShadow)
{
	m_mesh = cResourceManager::Get()->LoadXFile(renderer, fileName);

	m_shader = cResourceManager::Get()->LoadShader(renderer, "hlsl_xfile.fx");
	if (m_shader)
		m_shader->SetTechnique("Scene");

	return true;
}


void cXFileModel::Render(cRenderer &renderer, const Matrix44 &tm)
{
	if (m_mesh)
		m_mesh->Render(renderer, m_tm * tm);
}


void cXFileModel::RenderShader(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	if (m_mesh && m_shader)
		m_mesh->RenderShader(renderer, *m_shader, m_tm * tm);
}
