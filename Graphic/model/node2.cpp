
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
	, m_parent(NULL)
	, m_nodeType(type)
	, m_flags(1)
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
bool cNode2::RemoveChild(const int id)
{
	for (auto &node : m_children)
	{
		if (id == node->m_id)
		{
			delete node;
			common::removevector(m_children, node);
			return true;
		}
	}

	for (auto &node : m_children)
		if (node->RemoveChild(id))
			return true;

	return false;
}


bool cNode2::RemoveChild(cNode2 *rmNode)
{
	for (auto &node : m_children)
	{
		if (rmNode->m_id == node->m_id)
		{
			delete node;
			common::removevector(m_children, node);
			return true;
		}
	}

	for (auto &node : m_children)
		if (node->RemoveChild(rmNode->m_id))
			return true;

	return false;
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
