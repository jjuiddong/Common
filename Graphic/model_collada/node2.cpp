
#include "stdafx.h"
#include "node2.h"

using namespace graphic;


cNode2::cNode2()
	: m_id(0)
	, m_isVisible(true)
	, m_tmPalette(NULL)
{
}

cNode2::~cNode2()
{
	Clear();
}


bool cNode2::Create(cRenderer &renderer, const sRawNode &rawNode, vector<Matrix44> *tmPalette)
{
	Clear();

	m_id = rawNode.id;
	m_name = rawNode.name;
	m_localTm = rawNode.localTm;
	m_localTm.Transpose();
	m_tmPalette = tmPalette;

	m_meshes.reserve(rawNode.meshes.size());
	for (auto mesh : rawNode.meshes)
	{
		cMesh2 *p = new cMesh2();
		p->Create(renderer, mesh, tmPalette);
		m_meshes.push_back(p);
	}

	m_children.reserve(rawNode.children.size());
	for (auto child : rawNode.children)
	{
		cNode2 *p = new cNode2();
		p->Create(renderer, child, tmPalette);
		m_children.push_back(p);
	}

	return true;
}


void cNode2::Render(cRenderer &renderer, const Matrix44 &tm)
// tm = Matrix44::Identity
{
	RET(!m_isVisible);

	//(*m_tmPalette)[m_id] = m_localTm;
	(*m_tmPalette)[m_id] = m_localTm * tm;

	for (auto mesh : m_meshes)
		mesh->Render(renderer);

	for (auto child : m_children)
		child->Render(renderer, m_localTm * tm);
}


bool cNode2::Update(const float deltaSeconds, const Matrix44 &tm)
// tm = Matrix44::Identity
{
	for (auto mesh : m_meshes)
		mesh->Update(deltaSeconds);

	for (auto child: m_children)
		child->Update(deltaSeconds);

	return true;
}


void cNode2::Clear()
{
	for (auto mesh : m_meshes)
		delete mesh;
	m_meshes.clear();

	for (auto node : m_children)
		delete node;
	m_children.clear();
}
