
#include "stdafx.h"
#include "model_collada.h"

using namespace graphic;

cColladaModel::cColladaModel()
	: m_isSkinning(false)
{
}

cColladaModel::~cColladaModel()
{
	Clear();
}


bool cColladaModel::Create(cRenderer &renderer, const StrPath &fileName)
{
	Clear();

	sRawMeshGroup2 *rawMeshes = cResourceManager::Get()->LoadRawMesh2(fileName);
	RETV(!rawMeshes, false);

	//m_fileName = fileName;
	m_storedAnimationName = rawMeshes->animationName;

	m_isSkinning = !rawMeshes->bones.empty();
	if (!rawMeshes->bones.empty())
	{
		m_skeleton.Create(rawMeshes->bones);
	}

	for (auto &mesh : rawMeshes->meshes)
	{
		cMesh2 *p = new cMesh2();
		p->Create(renderer, mesh, &m_skeleton);
		m_meshes.push_back(p);
	}

	UpdateBoundingBox();

	return true;
}


bool cColladaModel::Render(cRenderer &renderer, cShader &shader, const Matrix44 &tm)
// tm = Matrix44:Identity
{
	const int passCount = shader.Begin();
	for (int i = 0; i < passCount; ++i)
	{
		shader.BeginPass(i);
		for (auto &mesh : m_meshes)
			mesh->RenderShader(renderer, &shader, tm);
		shader.EndPass();
	}
	shader.End();

	return true;
}


bool cColladaModel::Update(const float deltaSeconds)
{
	m_animation.Update(deltaSeconds);

	for (auto &mesh : m_meshes)
		mesh->Update(deltaSeconds);

	return true;
}


void cColladaModel::UpdateBoundingBox()
{
	sMinMax mm;
	for each (auto &mesh in m_meshes)
	{
		mm.Update(mesh->m_buffers->m_boundingBox.m_min);
		mm.Update(mesh->m_buffers->m_boundingBox.m_max);
	}

	m_boundingBox.SetBoundingBox(mm._min, mm._max);
	if (m_boundingBox.Length() >= FLT_MAX)
		m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(0, 0, 0));
}


void cColladaModel::SetAnimation(const Str64 &animationName, const bool isMerge)
// isMerge = false
{
	if (sRawAniGroup *rawAnies = cResourceManager::Get()->LoadAnimation(animationName.c_str()))
		m_animation.Create(*rawAnies, &m_skeleton, isMerge);
}


void cColladaModel::Clear()
{
	for (auto &mesh : m_meshes)
		delete mesh;
	m_meshes.clear();

	//m_shader = NULL;
}


//void cColladaModel::SetShader(cShader *shader) 
//{
//	m_shader = shader;
//}
