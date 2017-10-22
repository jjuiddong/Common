
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

	sRawMeshGroup2 *rawMeshes = cResourceManager::Get()->LoadRawMesh(fileName);
	RETV(!rawMeshes, false);

	m_nodes = rawMeshes->nodes;
	m_storedAnimationName = rawMeshes->animationName;

	m_isSkinning = !rawMeshes->bones.empty();
	if (!rawMeshes->bones.empty())
		m_skeleton.Create(rawMeshes);

	for (auto &mesh : rawMeshes->meshes)
	{
		cMesh *p = new cMesh();
		p->Create(renderer, mesh, &m_skeleton);
		m_meshes.push_back(p);
	}

	UpdateBoundingBox();

	// Create Animation
	sRawAniGroup *rawAnies = cResourceManager::Get()->LoadAnimation(rawMeshes->animationName.c_str());
	if (rawAnies)
		m_animation.Create(*rawAnies, &m_skeleton);
	else
		m_animation.Create(&m_skeleton); // empty animation

	return true;
}


bool cAssimpModel::Render(cRenderer &renderer
	, const char *techniqueName
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(m_nodes.empty(), false);
	RenderNode(renderer, techniqueName, m_nodes[0], XMIdentity, parentTm, flags);
	return true;
}


bool cAssimpModel::RenderInstancing(cRenderer &renderer
	, const char *techniqueName
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
		mesh->RenderInstancing(renderer, techniqueName, count, parentTm);
	}
	return true;
}


// Render From Node
bool cAssimpModel::RenderNode(cRenderer &renderer
	, const char *techniqueName
	, const sRawNode &node
	, const XMMATRIX &parentTm //= XMIdentity
	, const XMMATRIX &transformTm //= XMIdentity
	, const int flags //= 1
)
{
	const XMMATRIX tm = node.localTm.GetMatrixXM() * parentTm;

	// Render Meshes
	for (auto idx : node.meshes)
		m_meshes[idx]->Render(renderer, techniqueName, tm, transformTm);

	// Render Child Node
	for (auto idx : node.children)
		RenderNode(renderer, techniqueName, m_nodes[idx], tm, transformTm, flags);

	return true;
}


bool cAssimpModel::Update(const float deltaSeconds, const float incT)
{
	return m_animation.Update(incT);
}


void cAssimpModel::UpdateBoundingBox()
{
	RET(m_nodes.empty());

	sMinMax mm;
	struct sData {sRawNode *p; Matrix44 tm;};
	vector<sData> s; // like stack
	s.push_back({ &m_nodes[0] });

	while (!s.empty())
	{
		sRawNode *node = s.back().p;
		Matrix44 tm = s.back().tm;
		s.pop_back();

		for (auto idx : node->meshes)
		{
			cMesh *mesh = m_meshes[idx];
			if (!mesh->m_buffers)
				continue;
			cBoundingBox bbox = mesh->m_buffers->m_boundingBox;
			bbox *= node->localTm * tm;
			const Vector3 center = bbox.Center();
			const Vector3 dim = bbox.GetDimension();
			const Vector3 _min = center - (dim * 0.5f);
			const Vector3 _max = center + (dim * 0.5f);
			mm.Update(_min);
			mm.Update(_max);
		}

		for (auto idx : node->children)
			s.push_back({ &m_nodes[idx], node->localTm * tm});
	}

	m_boundingBox.SetBoundingBox(mm);
}


void cAssimpModel::SetAnimation(const Str64 &animationName, const bool isMerge)
// isMerge = false
{
	m_animation.SetAnimation(animationName, isMerge);
}


void cAssimpModel::Clear()
{
	for (auto &mesh : m_meshes)
		delete mesh;
	m_meshes.clear();
}

int cAssimpModel::GetVertexType()
{
	RETV(m_meshes.empty(), 0);
	RETV(!m_meshes.back()->m_buffers, 0);
	return m_meshes.back()->m_buffers->m_vtxType;
}
