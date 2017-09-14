
#include "stdafx.h"
#include "model_assimp.h"

using namespace graphic;


cAssimpModel::cAssimpModel()
	: m_isSkinning(false)
{
}

cAssimpModel::~cAssimpModel()
{
	Clear();
}


bool cAssimpModel::Create(cRenderer &renderer, const StrPath &fileName)
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


bool cAssimpModel::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	for (auto &mesh : m_meshes)
		mesh->Render(renderer, parentTm);
	return true;
}


bool cAssimpModel::RenderInstancing(cRenderer &renderer
	, const int count
	, const XMMATRIX *transforms
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	for (auto &mesh : m_meshes)
	{
		for (int i = 0; i < count; ++i)
		{
			const XMMATRIX &tm1 = transforms[i];
			const XMMATRIX &tm2 = mesh->m_transform.GetMatrixXM();
			renderer.m_cbInstancing.m_v->worlds[i] = XMMatrixTranspose(tm2 * tm1 * parentTm);
		}
		renderer.m_cbInstancing.Update(renderer, 3);
		mesh->RenderInstancing(renderer, count, parentTm);
	}
	return true;
}


bool cAssimpModel::Update(const float deltaSeconds)
{
	m_animation.Update(deltaSeconds);
	return true;
}


void cAssimpModel::UpdateBoundingBox()
{
	//sMinMax mm;
	//for each (auto &mesh in m_meshes)
	//{
	//	mm.Update(mesh->m_buffers->m_boundingBox.m_min);
	//	mm.Update(mesh->m_buffers->m_boundingBox.m_max);
	//}

	//m_boundingBox.SetBoundingBox(mm._min, mm._max);
	//if (m_boundingBox.Length() >= FLT_MAX)
	//	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(0, 0, 0));
}


void cAssimpModel::SetAnimation(const Str64 &animationName, const bool isMerge)
// isMerge = false
{
	//if (sRawAniGroup *rawAnies = cResourceManager::Get()->LoadAnimation(animationName.c_str()))
	//	m_animation.Create(*rawAnies, &m_skeleton, isMerge);
}


void cAssimpModel::Clear()
{
	for (auto &mesh : m_meshes)
		delete mesh;
	m_meshes.clear();

	//m_shader = NULL;
}


//void cAssimpModel::SetShader(cShader *shader) 
//{
//	m_shader = shader;
//}


int cAssimpModel::GetVertexType()
{
	RETV(m_meshes.empty(), 0);
	RETV(!m_meshes[0]->m_buffers, 0);
	return m_meshes[0]->m_buffers->m_vtxType;
}
