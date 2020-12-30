
#include "stdafx.h"
#include "pathfinder2.h"

using namespace ai;


cPathFinder2::cPathFinder2()
{
}

cPathFinder2::~cPathFinder2()
{
	Clear();
}


// add vertex
// return vertex index
int cPathFinder2::AddVertex(const sVertex &vtx)
{
	m_vertices.push_back(vtx);
	return m_vertices.size() - 1;
}


// add transition
// prop: transition property
bool cPathFinder2::AddTransition(const uint fromVtxIdx, const uint toVtxIdx
	, const int prop //=0
)
{
	RETV2((int)m_vertices.size() <= fromVtxIdx, false);
	RETV(fromVtxIdx  == toVtxIdx, false);

	sVertex &vtx = m_vertices[fromVtxIdx];
	if (IsExistTransition(fromVtxIdx, toVtxIdx))
		return false; // already linked

	sTransition tr;
	tr.to = toVtxIdx;
	tr.distance = 1.f;
	tr.w = 1.f;
	tr.prop = prop;
	tr.toWaypoint = 0;
	tr.enable = true;
	vtx.trs.push_back(tr);
	return true;
}


// is exist same transition?
bool cPathFinder2::IsExistTransition(const uint fromVtxIdx, const uint toVtxIdx)
{
	RETV2((int)m_vertices.size() <= fromVtxIdx, false);
	RETV(fromVtxIdx == toVtxIdx, false);
	sVertex &vtx = m_vertices[fromVtxIdx];
	for (auto &tr : vtx.trs)
		if (tr.to == toVtxIdx)
			return true;
	return false;
}


// remove transition
bool cPathFinder2::RemoveTransition(const uint fromVtxIdx, const uint toVtxIdx)
{
	RETV((int)m_vertices.size() <= fromVtxIdx, false);
	sVertex &vtx = m_vertices[fromVtxIdx];
	int idx = -1;
	for (uint i = 0; i < vtx.trs.size(); ++i)
	{
		if (vtx.trs[i].to == toVtxIdx)
		{
			idx = (int)i;
			break;
		}
	}
	if (idx < 0)
		return false; // not found transition
	common::popvector(vtx.trs, idx);
	return true;
}


// remove vertex
bool cPathFinder2::RemoveVertex(const uint vtxIdx)
{
	// remove edge and decrease index if greater than remove index
	for (auto &v : m_vertices)
		for (uint i=0; i < v.trs.size(); ++i)
			if (vtxIdx == v.trs[i].to)
			{
				common::popvector(v.trs, i);
				break;
			}
	for (auto &v : m_vertices)
		for (auto &tr : v.trs)
			if (vtxIdx < tr.to)
				--tr.to;

	// remove index vertex
	common::rotatepopvector(m_vertices, vtxIdx);
	return true;
}


// return vertex index from name
// if not found, return -1
int cPathFinder2::GetVertexId(const Str16 &name) const
{
	if (name.empty())
		return -1;
	for (uint i = 0; i < m_vertices.size(); ++i)
	{
		const auto &vtx = m_vertices[i];
		if (!vtx.name.empty() && (name == vtx.name))
			return (int)i;
	}
	return -1;
}


// reserver vertex buffer
void cPathFinder2::ReservedVertexBuffer(const uint vertexCount)
{
	m_vertices.reserve(vertexCount);
}


void cPathFinder2::Clear()
{
	m_vertices.clear();
}
