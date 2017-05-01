
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


bool cColladaModel::Create(cRenderer &renderer, const string &fileName)
{
	Clear();

	sRawMeshGroup2 *rawMeshes = cResourceManager::Get()->LoadModel2(fileName);
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
	for (auto &mesh : m_meshes)
		mesh->RenderShader(renderer, &shader, tm);

	//const Matrix44 transform = m_tm * tm;

	//if (m_shader)
	//{
	//	GetMainCamera()->Bind(*m_shader);
	//	GetMainLight().Bind(*m_shader);

	//	for (auto &mesh : m_meshes)
	//		mesh->RenderShader(renderer, m_shader, transform);

	//	if (m_shader->m_isReload)
	//		m_shader->m_isReload = false;
	//}
	//else
	//{
	//	for (auto &mesh : m_meshes)
	//		mesh->Render(renderer, transform);
	//}

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


void cColladaModel::SetAnimation(const string &animationName, const bool isMerge)
// isMerge = false
{
	if (sRawAniGroup *rawAnies = cResourceManager::Get()->LoadAnimation(animationName))
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
