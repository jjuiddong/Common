
#include "stdafx.h"
#include "mesh2.h"
#include "skeleton.h"

using namespace graphic;


cMesh2::cMesh2()
	: m_buffers(NULL)
	, m_skeleton(NULL)
{
}

cMesh2::~cMesh2()
{
	Clear();
}


// Create Mesh
bool cMesh2::Create(cRenderer &renderer, const sRawMesh2 mesh, cSkeleton *skeleton)
{
	Clear();

	m_name = mesh.name;
	m_skeleton = skeleton;
	m_bones = mesh.bones;
	m_localTm = mesh.localTm;

	CreateMaterials(renderer, mesh);
	
	m_buffers = new cMeshBuffer(renderer, mesh);

	m_tmPose.resize(m_bones.size());

	return true;
}


void cMesh2::CreateMaterials(cRenderer &renderer, const sRawMesh2 &rawMesh)
{
	m_mtrls.push_back(cMaterial());
	m_mtrls.back().Init(rawMesh.mtrl);

	if (rawMesh.mtrl.texture.empty())
	{
		m_colorMap.push_back(
			cResourceManager::Get()->LoadTexture(renderer, "", g_defaultTexture));
	}
	else
	{
		m_colorMap.push_back(
			cResourceManager::Get()->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.texture));
	}

	if (!rawMesh.mtrl.bumpMap.empty())
		m_normalMap.push_back(
			cResourceManager::Get()->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.bumpMap));

	if (!rawMesh.mtrl.specularMap.empty())
		m_specularMap.push_back(
			cResourceManager::Get()->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.specularMap));

	if (!rawMesh.mtrl.selfIllumMap.empty())
		m_selfIllumMap.push_back(
			cResourceManager::Get()->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.selfIllumMap));
}


void cMesh2::Render(cRenderer &renderer
	, const XMMATRIX &tm // = XMIdentity
)
{
	RET(!m_buffers);

	if (!m_mtrls.empty())
	{
		renderer.m_cbMaterial.m_v->ambient = XMLoadFloat4(&m_mtrls[0].m_ambient);
		renderer.m_cbMaterial.m_v->diffuse = XMLoadFloat4(&m_mtrls[0].m_diffuse);
		renderer.m_cbMaterial.m_v->specular = XMLoadFloat4(&m_mtrls[0].m_specular);
		renderer.m_cbMaterial.m_v->emissive = XMLoadFloat4(&m_mtrls[0].m_emissive);
		renderer.m_cbMaterial.m_v->pow = m_mtrls[0].m_power;
		renderer.m_cbMaterial.Update(renderer);

	}
	//	m_mtrls[0].Bind(renderer);
	if (!m_colorMap.empty())
		m_colorMap[0]->Bind(renderer, 0);

	const XMMATRIX m = m_transform.GetMatrixXM() * tm;
	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m);
	renderer.m_cbPerFrame.Update(renderer);
	m_buffers->Render(renderer);
}


// Shader Rendering
//void cMesh2::RenderShader(cRenderer &renderer, cShader *shader, const Matrix44 &tm)
//{
//	RET(!shader);
//	//RET(m_bones.empty());
//
//	if (!m_mtrls.empty())
//		m_mtrls[0].Bind(*shader);
//
//	shader->SetMatrix("g_mWorld", m_localTm * tm);
//
//	if (!m_colorMap.empty())
//		shader->SetTexture("g_colorMapTexture", *m_colorMap[0]);
//
//	// Set Skinning Information
//	for (u_int i = 0; i < m_bones.size(); ++i)
//		m_tmPose[i] = m_bones[i].offsetTm * m_skeleton->m_tmPose[m_bones[i].id];
//	if (!m_tmPose.empty())
//		shader->SetMatrixArray("g_mPalette", (Matrix44*)&m_tmPose[0], m_tmPose.size());
//	//
//
//	shader->CommitChanges();
//	m_buffers->Bind(renderer);
//	m_buffers->Render(renderer);
//
//
//
//	//if (!m_mtrls.empty())
//	//	m_mtrls[0].Bind(*shader);
//	//
//	//shader->SetMatrix("g_mWorld", m_localTm * tm);
//
//	//if (!m_colorMap.empty())
//	//	shader->SetTexture("g_colorMapTexture", *m_colorMap[0]);
//
//	//// Set Skinning Information
//	//for (u_int i = 0; i < m_bones.size(); ++i)
//	//	m_tmPose[i] = m_bones[i].offsetTm * m_skeleton->m_tmPose[ m_bones[i].id];
//	//if (!m_tmPose.empty())
//	//	shader->SetMatrixArray("g_mPalette", (Matrix44*)&m_tmPose[0], m_tmPose.size());
//	////
//
//	//const int passCount = shader->Begin();
//	//for (int i = 0; i < passCount; ++i)
//	//{
//	//	shader->BeginPass(i);
//	//	m_buffers->Bind(renderer);
//	//	shader->CommitChanges();
//	//	m_buffers->Render(renderer);
//	//	shader->EndPass();
//	//}
//	//shader->End();
//}


void cMesh2::Clear()
{
	SAFE_DELETE(m_buffers);

	m_mtrls.clear();
	m_colorMap.clear();
	m_normalMap.clear();
	m_specularMap.clear();
	m_selfIllumMap.clear();
}
