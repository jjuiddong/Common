
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


// find path
bool cPathFinder2::Find(const Vector3 &start, const Vector3 &end
	, OUT vector<Vector3> &out
	, const set<sEdge> *disableEdges //= nullptr
	, OUT vector<uint> *outTrackVertexIndices //= nullptr
	, OUT vector<sEdge> *outTrackEdges //= nullptr
)
{
	const int startIdx = GetNearestVertex(start);
	if (startIdx < 0)
		return false;
	const int endIdx = GetNearestVertex(end);
	if (endIdx < 0)
		return false;

	return Find(startIdx, endIdx, out, disableEdges, outTrackVertexIndices, outTrackEdges);
}


// find path
bool cPathFinder2::Find(const uint startIdx, const uint endIdx
	, OUT vector<Vector3> &out
	, const set<sEdge> *disableEdges //= nullptr
	, OUT vector<uint> *outTrackVertexIndices //= nullptr
	, OUT vector<sEdge> *outTrackEdges //= nullptr
)
{
	vector<uint> verticesIndices;
	Find(startIdx, endIdx, verticesIndices, disableEdges);

	if (outTrackVertexIndices)
		*outTrackVertexIndices = verticesIndices;

	// Store Tracking Edge
	if (outTrackEdges)
	{
		outTrackEdges->reserve(verticesIndices.size());
		for (int i = 0; i < (int)verticesIndices.size() - 1; ++i)
		{
			const int from = verticesIndices[i];
			const int to = verticesIndices[i + 1];
			outTrackEdges->push_back(sEdge(from, to));
		}
	}

	return true;
}


// find path, a-star algorithm
// reference: http://www.gisdeveloper.co.kr/?p=3897
bool cPathFinder2::Find(const uint startIdx, const uint endIdx
	, OUT vector<uint> &out
	, const set<sEdge> *disableEdges //= nullptr
)
{
	if (startIdx == endIdx)
		return true; // no path

	struct sNode {
		uint idx; // vertex idx
		uint prev; // previous idx
		float len; // length from start
		float h; // heuristic
		float tot; // total len
	};

	vector<sNode> open;
	vector<sNode> close;
	set<int> oset; // open node vertex index
	set<int> cset; // close node vertex index

	const sVertex &endVtx = m_vertices[endIdx];

	sNode start;
	start.idx = startIdx;
	start.prev = -1;
	start.len = 0;
	start.tot = 0;
	close.push_back(start);
	cset.insert(startIdx);

	bool isFind = false; // find path?
	while (1)
	{
		const sNode &node = close.back();
		if (node.idx == endIdx)
		{
			isFind = true;
			break; // finish
		}

		const sVertex &vtx0 = m_vertices[node.idx];
		for (auto &tr : vtx0.trs)
		{
			const auto it2 = cset.find(tr.to);
			if (cset.end() != it2)
				continue;

			const sVertex &vtx1 = m_vertices[tr.to];
			const auto it1 = oset.find(tr.to);
			if (oset.end() != it1)
			{
				sNode *n = nullptr;
				auto it = std::find_if(open.begin(), open.end()
					, [&](auto &a) {return a.idx == tr.to; });
				if (it != open.end())
				{
					n = &*it;
				}
				else
				{
					oset.erase(tr.to); // error occurred, exception process
					continue;
				}

				// update node info?
				const float len = node.len + tr.distance;
				const float tot = node.len + tr.distance + vtx1.pos.Distance(endVtx.pos);
				if (tot < n->tot)
				{
					n->prev = node.idx;
					n->len = len;
					n->tot = tot;
				}
			}
			else
			{
				sNode n;
				n.idx = tr.to;
				n.prev = node.idx;
				n.len = node.len + tr.distance;
				n.tot = node.len + tr.distance + vtx1.pos.Distance(endVtx.pos);
				open.push_back(n);
				oset.insert(tr.to);
			}
		}

		if (open.empty())
			break; // no candidate vertex, no path, finish

		// sorting open list, descending
		std::sort(open.begin(), open.end()
			, [](auto &a1, auto &a2) { return a1.tot > a2.tot; });

		close.push_back(open.back());
		cset.insert(open.back().idx);
		open.pop_back();
	}

	if (!isFind)
		return false; // not found path

	// backtracking
	sNode &node = close.back();
	while (1)
	{
		out.push_back(node.idx);
		if (node.idx == startIdx)
			break; // finish
		auto it = find_if(close.begin(), close.end()
			, [&](auto &a) {return a.idx == node.prev; });
		if (it == close.end())
			break; // error occurred
		node = *it;
	}

	std::reverse(out.begin(), out.end());

	return true;
}


// return = minimum( distance(pos, nearest vertex) )
//		    return vertex index
int cPathFinder2::GetNearestVertex(const Vector3 &pos) const
{
	RETV(m_vertices.empty(), -1);
	int idx = -1;
	float minLen = FLT_MAX;
	for (uint i = 0; i < m_vertices.size(); ++i)
	{
		auto &vtx = m_vertices[i];
		const float len = vtx.pos.LengthRoughly(pos);
		if (len < minLen)
		{
			idx = (int)i;
			minLen = len;
		}
	}
	return idx;
}


// add vertex
// return vertex index
uint cPathFinder2::AddVertex(const sVertex &vtx)
{
	m_vertices.push_back(vtx);
	const uint idx = m_vertices.size() - 1;
	const int id = atoi(vtx.name.c_str());
	m_vertices[idx].id = id;
	m_vtxMap[id] = idx;
	return idx;
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
			if (vtxIdx < (uint)tr.to)
				--tr.to;

	// remove index vertex
	common::rotatepopvector(m_vertices, vtxIdx);

	// update vertex id-index mapping
	m_vtxMap.clear();
	for (uint i=0; i < m_vertices.size(); ++i)
	{
		auto &vtx = m_vertices[i];
		m_vtxMap[vtx.id] = i;
	}

	return true;
}


// return vertex by index
cPathFinder2::sVertex* cPathFinder2::GetVertexByIndex(const uint vtxIdx)
{
	if (m_vertices.size() <= vtxIdx)
		return nullptr;
	return &m_vertices[vtxIdx];
}


// return vertex by name
cPathFinder2::sVertex* cPathFinder2::GetVertexByName(const Str16 &name)
{
	const int idx = GetVertexIndexByName(name);
	if (idx < 0)
		return nullptr;
	return &m_vertices[idx];
}


// return vertex by id
cPathFinder2::sVertex* cPathFinder2::GetVertexByID(const int id)
{
	const int idx = GetVertexIndexByID(id);
	if (idx < 0)
		return nullptr;
	return &m_vertices[idx];
}


// return vertex index from name
// if not found, return -1
int cPathFinder2::GetVertexIndexByName(const Str16 &name) const
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


// return vertex index by vertex id
//		  -1:not found
int cPathFinder2::GetVertexIndexByID(const int id)
{
	auto it = m_vtxMap.find(id);
	if (m_vtxMap.end() == it)
		return -1;
	return it->second;
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
