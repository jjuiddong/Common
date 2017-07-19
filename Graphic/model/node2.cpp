
#include "stdafx.h"
#include "node2.h"

using namespace graphic;


cNode2::cNode2(const int id
	, const StrId &name // = "none"
	, const eNodeType::Enum type //= NONE
) 
	: m_id(id)
	, m_name(name)
	, m_isEnable(true)
	, m_isShow(true)
	, m_isShadowEnable(true)
	, m_isShadow(true)
	, m_parent(NULL)
	, m_type(type)
	, m_flags(1)
	, m_shader(NULL)
{
}

cNode2::~cNode2()
{
	Clear();
}


// 자식노드의 Render 를 호출한다.
bool cNode2::Render(cRenderer &renderer
	, const Matrix44 &parentTm // = Matrix44::Identity
	, const int flags //= 1
)
{
	RETV(!m_isEnable, false);
	RETV(!m_isShow, false);

	const Matrix44 tm = m_transform.GetMatrix() * parentTm;

	for (auto &node : m_children)
		node->Render(renderer, tm, flags);

	// for Debugging
	if (renderer.m_isDbgRender)
	{
		if (0 == renderer.m_dbgRenderStyle)
		{
			renderer.m_dbgSphere.m_transform.pos = m_boundingSphere.m_pos;
			renderer.m_dbgSphere.m_transform.scale = Vector3(1, 1, 1) * m_boundingSphere.m_radius;
			renderer.m_dbgSphere.Render(renderer, tm);
		}
		else
		{
			renderer.m_dbgBox.SetBox(m_boundingBox.m_min, m_boundingBox.m_max);
			renderer.m_dbgBox.Render(renderer, tm);
		}
	}

	return true;
}


bool cNode2::Update(cRenderer &renderer, const float deltaSeconds)
{
	RETV(!m_isEnable, false);
	RETV(!m_isShow, false);

	for (auto &node : m_children)
		node->Update(renderer, deltaSeconds);

	return true;
}


// 노드를 자식으로 추가한다.
bool cNode2::AddChild(cNode2 *node)
{
	assert(node);
	assert(!FindNode(node->m_id));

	RETV(!node, false);
	RETV(FindNode(node->m_id), false); // Already Exist

	node->m_parent = this;
	m_children.push_back(node);
	return true;
}


// 자식 노드 중에 id 와 같은 노드가 있다면 리턴한다.
const cNode2* cNode2::FindNode(const int id) const
{
	RETV(m_id == id, this);

	for (auto &node : m_children)
	{
		if (const cNode2 *ret = node->FindNode(id))
			return ret;
	}

	return NULL;
}


// 자식 노드 중에 name과 같은 노드가 있다면 리턴한다.
const cNode2* cNode2::FindNode(const StrId &name) const
{
	if (m_name == name)
		return this;

	for (auto &node : m_children)
		if (const cNode2 *ret = node->FindNode(name))
			return ret;

	return NULL;
}

// id 노드를 제거한다. 메모리까지 제거된다.
bool cNode2::RemoveChild(const int id
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


bool cNode2::RemoveChild(cNode2 *rmNode
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


void cNode2::CalcBoundingSphere()
{
	m_boundingSphere.Set(m_boundingBox);
}


float cNode2::CullingTest(const cFrustum &frustum
	, const Matrix44 &tm //= Matrix44::Identity
	, const bool isModel //= true
)
{
	RETV(!m_isEnable, false);

	const Matrix44 transform = m_transform.GetMatrix()*tm;
	if (frustum.IsInSphere(m_boundingSphere, transform))
	{
		m_isShow = true;
		m_isShadow = m_isShadowEnable;

		if (isModel)
		{
			for (auto &p : m_children)
				p->CullingTest(frustum, transform, isModel);
		}
		else
		{
			for (auto &p : m_children)
				p->m_isShow = true;
		}
	}
	else
	{
		m_isShow = false;
	}

	return frustum.m_pos.LengthRoughly(m_boundingBox.Center() * transform);
}


cNode2* cNode2::Picking(const Vector3 &orig, const Vector3 &dir, const eNodeType::Enum type)
{
	if (type == m_type)
		if (m_boundingBox.Pick(orig, dir, GetWorldMatrix()))
			return this;

	for (auto &p : m_children)
		if (cNode2 *n = p->Picking(orig, dir, type))
			return n;

	return NULL;
}


// 모든 노드를 제거한다.
void cNode2::Clear()
{
	for (auto &node : m_children)
	{
		node->Clear();
		delete node;
	}

	m_children.clear();
}


Matrix44 cNode2::GetWorldMatrix() const
{
	Matrix44 ret = m_transform.GetMatrix();
	cNode2 *node = m_parent;
	while (node)
	{
		ret *= node->m_transform.GetMatrix();
		node = node->m_parent;
	}
	return ret;
}
