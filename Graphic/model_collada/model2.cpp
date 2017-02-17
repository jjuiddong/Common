
#include "stdafx.h"
#include "model2.h"

using namespace graphic;

cModel2::cModel2()
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

	m_storedAnimationName = rawMeshes->animationName;

	m_skeleton.Create(rawMeshes->bones);

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
{
	for (auto &mesh : m_meshes)
		mesh->Render(renderer, tm);

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
}
