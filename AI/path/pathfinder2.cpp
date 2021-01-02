
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
	, OUT vector<int> *outTrackVertexIndices //= nullptr
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
bool cPathFinder2::Find(const int startIdx, const int endIdx
	, OUT vector<Vector3> &out
	, const set<sEdge> *disableEdges //= nullptr
	, OUT vector<int> *outTrackVertexIndices //= nullptr
	, OUT vector<sEdge> *outTrackEdges //= nullptr
)
{
	vector<int> verticesIndices;
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


// find path
bool cPathFinder2::Find(const int startIdx, const int endIdx
	, OUT vector<int> &out
	, const set<sEdge> *disableEdges //= nullptr
)
{
	const Vector3 start = m_vertices[startIdx].pos;
	const Vector3 end = m_vertices[endIdx].pos;

	set<int> visitSet;
	map<int, set<int>> vtxEdges;
	m_lenSet.clear();

	//vector<int> candidate;
	//candidate.reserve(m_vertices.size());
	//candidate.push_back(startIdx);
	//m_vertices[startIdx].startLen = 0;
	//m_vertices[startIdx].endLen = Distance(start, end);

	//int loopCount1 = 0; // debug, loop count
	//int loopCount2 = 0; // debug, insertion count
	//bool isFind = false;
	//while (!candidate.empty())
	//{
	//	const int curIdx = candidate.front();
	//	rotatepopvector(candidate, 0);

	//	sVertex &curVtx = m_vertices[curIdx];

	//	if (endIdx == curIdx)
	//	{
	//		isFind = true;
	//		break;
	//	}

	//	for (int i = 0; i < sVertex::MAX_EDGE; ++i)
	//	{
	//		if (curVtx.edge[i].to < 0)
	//			break;

	//		const int nextIdx = curVtx.edge[i].to;
	//		sVertex &nextVtx = m_vertices[nextIdx];

	//		const int edgeKey1 = MakeEdgeKey(curIdx, nextIdx);
	//		const int edgeKey2 = MakeEdgeKey(nextIdx, curIdx);
	//		if (visitSet.end() != visitSet.find(edgeKey1))
	//			continue; // is visit?

	//		if (disableEdges)
	//		{
	//			if (disableEdges->end() != disableEdges->find(sEdge(curIdx, nextIdx)))
	//				continue;
	//		}

	//		nextVtx.startLen = curVtx.startLen
	//			+ Distance(curVtx.pos, nextVtx.pos) * curVtx.edge[i].w + 0.00001f;
	//		nextVtx.endLen = Distance(end, nextVtx.pos);

	//		vtxEdges[curIdx].insert(nextIdx);
	//		vtxEdges[nextIdx].insert(curIdx);
	//		visitSet.insert(edgeKey1);
	//		visitSet.insert(edgeKey2);
	//		m_lenSet[edgeKey1] = nextVtx.startLen + nextVtx.endLen;
	//		m_lenSet[edgeKey2] = nextVtx.startLen + nextVtx.endLen;

	//		// sorting candidate
	//		// value = minimum( startLen + endLen )
	//		bool isInsert = false;
	//		for (uint k = 0; k < candidate.size(); ++k)
	//		{
	//			++loopCount1;

	//			sVertex &compVtx = m_vertices[candidate[k]];
	//			if ((compVtx.endLen + compVtx.startLen)
	//		> (nextVtx.endLen + nextVtx.startLen))
	//			{
	//				++loopCount2;

	//				candidate.push_back(nextIdx);
	//				common::rotateright2(candidate, k);
	//				isInsert = true;
	//				break;
	//			}
	//		}

	//		if (!isInsert)
	//			candidate.push_back(nextIdx);
	//	}
	//}

	//if (!isFind)
	//	return false;

	//// backward tracking
	//// end point to start point
	//out.push_back(endIdx);

	//visitSet.clear();

	//int curIdx = endIdx;
	//while ((curIdx != startIdx) && (out.size() < 1000))
	//{
	//	float minEdge = FLT_MAX;
	//	int nextIdx = -1;
	//	sVertex &vtx = m_vertices[curIdx];
	//	set<int> edges = vtxEdges[curIdx];
	//	for (auto next : edges)
	//	{
	//		const int edgeKey = MakeEdgeKey(curIdx, next);
	//		if (visitSet.end() != visitSet.find(edgeKey))
	//			continue; // is visit?

	//		auto it = m_lenSet.find(edgeKey);
	//		if (m_lenSet.end() == it)
	//			continue;

	//		if (minEdge > it->second)
	//		{
	//			minEdge = it->second;
	//			nextIdx = next;
	//		}
	//	}

	//	if (nextIdx < 0)
	//	{
	//		assert(0);
	//		break; // error occur
	//	}

	//	visitSet.insert(MakeEdgeKey(curIdx, nextIdx));
	//	visitSet.insert(MakeEdgeKey(nextIdx, curIdx));
	//	out.push_back(nextIdx);
	//	curIdx = nextIdx;
	//}

	//assert(out.size() < 1000);

	//std::reverse(out.begin(), out.end());

	//if (!out.empty() && (out.back() != end))
	//	out.push_back(end);

	return true;
}


// return = minimum( distance(pos, nearest vertex) )
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
			if (vtxIdx < (uint)tr.to)
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
