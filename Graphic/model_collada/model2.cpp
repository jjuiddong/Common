
#include "stdafx.h"
#include "model2.h"

using namespace graphic;

cModel2::cModel2()
	: m_shader(NULL)
{
}

cModel2::~cModel2()
{
	Clear();
}


bool cModel2::Create(cRenderer &renderer, const string &fileName)
{
	Clear();

	sRawMeshGroup2 *rawMeshes = cResourceManager::Get()->LoadModel2(fileName);
	RETV(!rawMeshes, false);

	m_fileName = fileName;
	m_storedAnimationName = rawMeshes->animationName;
	m_shader = cResourceManager::Get()->LoadShader(renderer, "hlsl_collada.fx");

	if (rawMeshes->bones.empty())
	{
		m_shader->SetTechnique("Rigid");
	}
	else
	{
		m_shader->SetTechnique("Skinning");
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


bool cModel2::Render(cRenderer &renderer, const Matrix44 &tm)
// tm = Matrix44:Identity
{
	const Matrix44 transform = m_tm * tm;

	if (m_shader)
	{
		GetMainCamera()->Bind(*m_shader);
		GetMainLight().Bind(*m_shader);

		for (auto &mesh : m_meshes)
			mesh->RenderShader(renderer, m_shader, transform);

		if (m_shader->m_isReload)
			m_shader->m_isReload = false;
	}
	else
	{
		for (auto &mesh : m_meshes)
			mesh->Render(renderer, transform);
	}

	return true;
}


bool cModel2::Update(const float deltaSeconds)
{
	m_animation.Update(deltaSeconds);

	for (auto &mesh : m_meshes)
		mesh->Update(deltaSeconds);

	return true;
}


void cModel2::UpdateBoundingBox()
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


void cModel2::SetAnimation(const string &animationName, const bool isMerge)
// isMerge = false
{
	if (sRawAniGroup *rawAnies = cResourceManager::Get()->LoadAnimation(animationName))
		m_animation.Create(*rawAnies, &m_skeleton, isMerge);
}


void cModel2::Clear()
{
	for (auto &mesh : m_meshes)
		delete mesh;
	m_meshes.clear();

	m_shader = NULL;
}


void cModel2::SetShader(cShader *shader) 
{
	m_shader = shader;
}
