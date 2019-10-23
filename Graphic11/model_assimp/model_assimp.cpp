
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
		p->Create(renderer, mesh);
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
	, cSkeleton *skeleton
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(m_nodes.empty(), false);

	RenderNode(renderer, techniqueName, skeleton
		, m_nodes[0], XMIdentity, parentTm, (eRenderFlag::NOALPHABLEND));

	if (HasAlphaBlend())
	{
		// 알파 블랜딩 메쉬는 컬링을 하지 않는다.
		CommonStates state(renderer.GetDevice());
		renderer.GetDevContext()->RSSetState(state.CullNone());

		RenderNode(renderer, techniqueName, skeleton
			, m_nodes[0], XMIdentity, parentTm, eRenderFlag::ALPHABLEND);

		renderer.GetDevContext()->RSSetState(state.CullCounterClockwise());
	}

	return true;
}


bool cAssimpModel::RenderInstancing(cRenderer &renderer
	, const char *techniqueName
	, cSkeleton *skeleton
	, const int count
	, const Matrix44 *transforms
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(m_nodes.empty(), false);

	RenderNode_Instacing(renderer, techniqueName, skeleton, m_nodes[0]
		, count, transforms, XMIdentity, parentTm, (eRenderFlag::NOALPHABLEND));
	return true;
}


bool cAssimpModel::RenderTessellation(cRenderer &renderer
	, const char *techniqueName
	, const int controlPointCount
	, cSkeleton *skeleton
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(m_nodes.empty(), false);

	RenderNode_Tessellation(renderer, techniqueName, controlPointCount
		, skeleton, m_nodes[0], XMIdentity, parentTm, (eRenderFlag::NOALPHABLEND));

	if (HasAlphaBlend())
	{
		// 알파 블랜딩 메쉬는 컬링을 하지 않는다.
		CommonStates state(renderer.GetDevice());
		renderer.GetDevContext()->RSSetState(state.CullNone());

		RenderNode_Tessellation(renderer, techniqueName, controlPointCount
			, skeleton, m_nodes[0], XMIdentity, parentTm, eRenderFlag::ALPHABLEND);

		renderer.GetDevContext()->RSSetState(state.CullCounterClockwise());
	}
	return true;
}


// Render From Node
bool cAssimpModel::RenderNode(cRenderer &renderer
	, const char *techniqueName
	, cSkeleton *skeleton
	, const sRawNode &node
	, const XMMATRIX &nodeParentTm //= XMIdentity
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	const XMMATRIX tm = node.localTm.GetMatrixXM() * nodeParentTm;

	// Render Meshes
	for (auto idx : node.meshes)
		if (m_meshes[idx]->m_renderFlags & flags)
			m_meshes[idx]->Render(renderer, techniqueName, skeleton, tm, parentTm);

	// Render Child Node
	for (auto idx : node.children)
		RenderNode(renderer, techniqueName, skeleton, m_nodes[idx], tm, parentTm, flags);

	return true;
}


bool cAssimpModel::RenderNode_Instacing(cRenderer &renderer
	, const char *techniqueName
	, cSkeleton *skeleton
	, const sRawNode &node
	, const int count
	, const Matrix44 *transforms
	, const XMMATRIX &nodeParentTm //= XMIdentity
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	const XMMATRIX tm = node.localTm.GetMatrixXM() * nodeParentTm;

	// Render Meshes
	for (auto idx : node.meshes)
		if (m_meshes[idx]->m_renderFlags & flags)
			m_meshes[idx]->RenderInstancing(renderer, techniqueName, skeleton
				, count, transforms, tm, parentTm);

	// Render Child Node
	for (auto idx : node.children)
		RenderNode_Instacing(renderer, techniqueName, skeleton, m_nodes[idx]
			, count, transforms, tm, parentTm, flags);
	return true;
}



// Render From Node
bool cAssimpModel::RenderNode_Tessellation(cRenderer &renderer
	, const char *techniqueName
	, const int controlPointCount
	, cSkeleton *skeleton
	, const sRawNode &node
	, const XMMATRIX &parentTm //= XMIdentity
	, const XMMATRIX &transformTm //= XMIdentity
	, const int flags //= 1
)
{
	const XMMATRIX tm = node.localTm.GetMatrixXM() * parentTm;

	// Render Meshes
	for (auto idx : node.meshes)
		if (m_meshes[idx]->m_renderFlags & flags)
			m_meshes[idx]->RenderTessellation(renderer, techniqueName
				, controlPointCount, skeleton, tm, transformTm);

	// Render Child Node
	for (auto idx : node.children)
		RenderNode_Tessellation(renderer, techniqueName
			, controlPointCount, skeleton, m_nodes[idx], tm, transformTm, flags);

	return true;
}



bool cAssimpModel::Update(const float deltaSeconds)
{
	return true;
}


void cAssimpModel::UpdateBoundingBox()
{
	RET(m_nodes.empty());

	sMinMax mm;
	struct sData {sRawNode *p; Matrix44 tm;};
	vector<sData> s; // like stack
	s.push_back({ &m_nodes[0], Matrix44::Identity });

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

			// todo: something wrong~ check boundingbox calculation
			// bounding orientation has garbage memory
			if ((_min.Length() > 1000000) || (_max.Length() > 1000000))
				continue;

			mm.Update(_min);
			mm.Update(_max);
		}

		for (auto idx : node->children)
			s.push_back({ &m_nodes[idx], node->localTm * tm});
	}

	m_boundingBox.SetBoundingBox(mm);
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


bool cAssimpModel::HasAlphaBlend()
{
	for (auto &mesh : m_meshes)
		if (mesh->m_renderFlags & eRenderFlag::ALPHABLEND)
			return true;
	return false;
}
