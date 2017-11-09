
#include "stdafx.h"
#include "node.h"

using namespace graphic;


cNode::cNode()
	: m_id(common::GenerateId())
	, m_name("none")
	, m_type(eNodeType::MODEL)
	, m_isEnable(true)
	, m_parent(NULL)
	, m_renderFlags(eRenderFlag::VISIBLE | eRenderFlag::MODEL | eRenderFlag::SHADOW)
	, m_opFlags(eOpFlag::COLLISION | eOpFlag::PICK)
	, m_shader(NULL)
	, m_techniqueName("Unlit")
{
	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion());
}

cNode::cNode(const int id
	, const StrId &name // = "none"
	, const eNodeType::Enum type //= NONE
) 
	: m_id(id)
	, m_name(name)
	, m_isEnable(true)
	, m_parent(NULL)
	, m_type(type)
	, m_renderFlags(eRenderFlag::VISIBLE | eRenderFlag::MODEL | eRenderFlag::SHADOW)
	, m_opFlags(eOpFlag::COLLISION | eOpFlag::PICK)
	, m_shader(NULL)
	, m_techniqueName("Unlit")
{
	m_boundingBox.SetBoundingBox(Vector3(0, 0, 0), Vector3(1, 1, 1), Quaternion());
}

cNode::~cNode()
{
	Clear();
}


// 자식노드의 Render 를 호출한다.
bool cNode::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);

	const XMMATRIX tm = m_transform.GetMatrixXM() * parentTm;

	for (auto &node : m_children)
		node->Render(renderer, tm, flags);

	// for Debugging
	if (renderer.m_isDbgRender)
	{
		if (0 == renderer.m_dbgRenderStyle) // sphere
		{
			renderer.m_dbgSphere.SetPos(m_boundingSphere.GetPos());
			renderer.m_dbgSphere.SetRadius(m_boundingSphere.GetRadius());
			renderer.m_dbgSphere.Render(renderer, tm);
			renderer.m_dbgAxis.SetAxis(m_boundingBox);
			renderer.m_dbgAxis.Render(renderer, tm);
		}
		else if (1 == renderer.m_dbgRenderStyle) // box
		{
			renderer.m_dbgBox.SetBox(m_boundingBox);
			renderer.m_dbgBox.Render(renderer, tm);
			renderer.m_dbgAxis.SetAxis(m_boundingBox);
			renderer.m_dbgAxis.Render(renderer, tm);
		}
	}

	return true;
}


bool cNode::RenderInstancing(cRenderer &renderer
	, const int count
	, const XMMATRIX *transforms
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);

	const XMMATRIX tm = m_transform.GetMatrixXM() * parentTm;

	for (auto &node : m_children)
		node->RenderInstancing(renderer, count, transforms, tm, flags);

	// for Debugging
	if (renderer.m_isDbgRender)
	{
		if (0 == renderer.m_dbgRenderStyle) // sphere
		{
			//renderer.m_dbgSphere.m_transform.pos = m_boundingSphere.m_pos;
			//renderer.m_dbgSphere.m_transform.scale = Vector3(1, 1, 1) * m_boundingSphere.m_radius;
			//renderer.m_dbgSphere.Render(renderer, tm);
		}
		else
		{
			//renderer.m_dbgBox.SetBox(m_boundingBox.m_min, m_boundingBox.m_max);
			//renderer.m_dbgBox.Render(renderer, tm);
		}

		//renderer.m_dbgAxis.SetAxis(m_boundingBox);
		//renderer.m_dbgAxis.Render(renderer, tm);
	}

	return true;
}


bool cNode::Update(cRenderer &renderer, const float deltaSeconds)
{
	RETV(!m_isEnable, false);
	RETV(!IsVisible(), false);

	for (auto &node : m_children)
		node->Update(renderer, deltaSeconds);

	return true;
}


// 노드를 자식으로 추가한다.
bool cNode::AddChild(cNode *node)
{
	RETV2(!node, false);
	RETV2(FindNode(node->m_id), false); // Already Exist

	if (node->m_parent)
		node->m_parent->RemoveChild(node, false);

	node->m_parent = this;
	m_children.push_back(node);
	return true;
}


// 자식 노드 중에 id 와 같은 노드가 있다면 리턴한다.
cNode* cNode::FindNode(const int id)
{
	RETV(m_id == id, this);

	for (auto &node : m_children)
	{
		if (cNode *ret = node->FindNode(id))
			return ret;
	}

	return NULL;
}


// 자식 노드 중에 name과 같은 노드가 있다면 리턴한다.
cNode* cNode::FindNode(const StrId &name)
{
	if (m_name == name)
		return this;

	for (auto &node : m_children)
		if (cNode *ret = node->FindNode(name))
			return ret;

	return NULL;
}

// id 노드를 제거한다. 메모리까지 제거된다.
bool cNode::RemoveChild(const int id
	, const bool rmInstance //= true
)
{
	for (auto &node : m_children)
	{
		if (id == node->m_id)
		{
			if (rmInstance)
				delete node;

			common::removevector(m_children, node);
			return true;
		}
	}

	for (auto &node : m_children)
		if (node->RemoveChild(id, rmInstance))
			return true;

	return false;
}


bool cNode::RemoveChild(cNode *rmNode
	, const bool rmInstance //= true
)
{
	for (auto &node : m_children)
	{
		if (rmNode->m_id == node->m_id)
		{
			if (rmInstance)
				delete node;

			rmNode->m_parent = NULL;
			common::removevector(m_children, node);
			return true;
		}
	}

	for (auto &node : m_children)
		if (node->RemoveChild(rmNode->m_id, rmInstance))
			return true;

	return false;
}


void cNode::CalcBoundingSphere()
{
	m_boundingSphere.SetBoundingSphere(m_boundingBox);
}


float cNode::CullingTest(const cFrustum &frustum
	, const XMMATRIX &tm //= XMIdentity
	, const bool isModel //= true
)
{
	RETV(!m_isEnable, -1.f);

	const XMMATRIX transform = m_transform.GetMatrixXM() * tm;
	const cBoundingSphere bsphere = m_boundingSphere * transform;
	if (frustum.IsInSphere(bsphere))
	{
		SetRenderFlag(eRenderFlag::VISIBLE, true);

		if (isModel)
		{
			for (auto &p : m_children)
				p->CullingTest(frustum, transform, isModel);
		}
		else
		{
			for (auto &p : m_children)
				p->SetRenderFlag(eRenderFlag::VISIBLE, true);
		}
	}
	else
	{
		SetRenderFlag(eRenderFlag::VISIBLE, false);
		return -1.f;
	}

	return frustum.LengthRoughly(m_boundingBox.Center() * transform);
}


cNode* cNode::Picking(const Ray &ray, const eNodeType::Enum type
	, const XMMATRIX &parentTm //= XMIdentity
	, const bool isSpherePicking //= true
)
{
	if (!(m_opFlags & eOpFlag::COLLISION))
		return NULL;

	const XMMATRIX tm = m_transform.GetMatrixXM() * parentTm;

	if (type == m_type)
	{
		cBoundingBox bbox = m_boundingBox;
		bbox *= tm;
		if (isSpherePicking)
		{
			cBoundingSphere bsphere;
			bsphere.SetBoundingSphere(bbox);
			if (bsphere.Intersects(ray))
				return this;
		}
		else
		{
			if (bbox.Pick(ray.orig, ray.dir))
				return this;
		}
	}

	if (m_children.empty())
		return NULL;

	vector<cNode*> picks;
	picks.reserve(4);

	for (auto &p : m_children)
		if (cNode *n = p->Picking(ray, type, tm))
			picks.push_back(n);

	if (picks.empty())
		return NULL;

	if (picks.size() == 1)
		return picks[0];

	cNode *mostNearest = NULL;
	float nearLen = FLT_MAX;
	for (auto &p : picks)
	{
		const Vector3 modelPos = p->GetWorldMatrix().GetPosition();
		Plane ground(Vector3(0, 1, 0), modelPos);
		const Vector3 pickPos = ground.Pick(ray.orig, ray.dir);
		const float len = modelPos.LengthRoughly(pickPos);
		if (nearLen > len)
		{
			nearLen = len;
			mostNearest = p;
		}
	}

	return mostNearest;
}


cNode* cNode::Picking(const Ray &ray, const eNodeType::Enum type
	, const bool isSpherePicking //= true
)
{
	const XMMATRIX parentTm = GetParentWorldMatrix().GetMatrixXM();
	return Picking(ray, type, parentTm, isSpherePicking);
}


// 모든 노드를 제거한다.
void cNode::Clear()
{
	for (auto &node : m_children)
	{
		node->Clear();
		delete node;
	}

	m_children.clear();
}


Matrix44 cNode::GetWorldMatrix() const
{
	Matrix44 ret = m_transform.GetMatrix();
	cNode *node = m_parent;
	while (node)
	{
		ret *= node->m_transform.GetMatrix();
		node = node->m_parent;
	}
	return ret;
}


Matrix44 cNode::GetParentWorldMatrix() const
{
	return (m_parent) ? m_parent->GetWorldMatrix() : Matrix44::Identity;
}


Transform cNode::GetWorldTransform() const
{
	Transform ret = m_transform;
	cNode *node = m_parent;
	while (node)
	{
		ret *= node->m_transform;
		node = node->m_parent;
	}
	return ret;
}


void cNode::SetTechnique(const char *techniqName
	, const bool isApplyChild // = true
)
{
	m_techniqueName = techniqName;

	if (isApplyChild)
	{
		for (auto &p : m_children)
			p->SetTechnique(techniqName, isApplyChild);
	}
}
