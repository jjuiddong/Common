
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

	out.reserve(verticesIndices.size());
	for (auto idx : verticesIndices)
		out.push_back(m_vertices[idx].pos);

	return true;
}


// find path, a-star algorithm
// reference: http://www.gisdeveloper.co.kr/?p=3897
// isChangeDirPenalty: add change direction penalty?
bool cPathFinder2::Find(const uint startIdx, const uint endIdx
	, OUT vector<uint> &out
	, const set<sEdge> *disableEdges //= nullptr
	, const bool isChangeDirPenalty //= false
	, const sTarget* target //= nullptr
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
	start.prev = UINT_MAX;
	start.len = 0;
	start.tot = 0;
	close.push_back(start);
	cset.insert(startIdx);

	sTargetArg arg;
	if (target)
	{
		arg.startIdx = startIdx;
		arg.endIdx = endIdx;
		arg.targetDist = target->target.Length();
		arg.target = target;
	}

	bool isFind = false; // find path?
	while (1)
	{
		const sNode &node = close.back();

		// target process?
		if (target)
		{
			arg.from = node.prev;
			arg.to = node.idx;
			const int res = ArriveTarget(arg);
			if ((1 == res) || (2 == res))
			{
				if (2 == res)
				{
					for (int i = (close.size() - 1); i >= 0; --i)
					{
						if (close.back().idx == arg.from) break;
						close.pop_back();
					}
				}
				isFind = true;
				break;
			}
		}
		else if (node.idx == endIdx)
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

			// calc direction change penalty
			float penalty = 0.f;
			if (isChangeDirPenalty && (UINT_MAX != node.prev))
			{
				const sVertex& vtx2 = m_vertices[node.prev];
				const Vector3 dir0 = (vtx0.pos - vtx2.pos).Normal();
				const Vector3 dir1 = (vtx1.pos - vtx0.pos).Normal();
				if (dir0.DotProduct(dir1) < 0.7f)
					penalty = 2.0f;
			}
			//~

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
				const float len = node.len + tr.distance + penalty;
				const float tot = node.len + tr.distance + vtx1.pos.Distance(endVtx.pos) + penalty;
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
				n.len = node.len + tr.distance + penalty;
				n.tot = node.len + tr.distance + vtx1.pos.Distance(endVtx.pos) + penalty;
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


// check arrive target position
// arge: argument
// return:	0 = no destination
//			1 = to destination
//			2 = from destination
inline int cPathFinder2::ArriveTarget(sTargetArg& arg)
{
	const Vector3& destPos = m_vertices[arg.endIdx].pos;
	const Vector3& toPos = m_vertices[arg.to].pos;
	Vector3 frPos;
	Vector3 objDir;

	if (UINT_MAX == arg.from)
	{
		frPos = toPos;
		objDir = (destPos - toPos).Normal();
	}
	else
	{
		frPos = m_vertices[arg.from].pos;
		objDir = (toPos - frPos).Normal();
	}

	if ((destPos.Distance(frPos) > arg.targetDist)
		&& (destPos.Distance(toPos) > arg.targetDist))
		return 0;

	// tricky code: only support OpenGL space
	// OpenGL -> DX
	const Vector3 f = arg.target->dir.ToOpenGL();// (arg.target->dir.x, arg.target->dir.y, -arg.target->dir.z);
	const Vector3 dir = objDir.ToOpenGL();// (objDir.x, objDir.y, -objDir.z);
	const Vector3 local = arg.target->target.ToOpenGL();// (arg.target->target.x, arg.target->target.y, -arg.target->target.z);
	Quaternion rot;
	rot.SetRotationArc(f, dir, Vector3(0, 1, 0));
	const Vector3 localTarget = local * rot;
	const Vector3 target = toPos + localTarget.ToOpenGL();// Vector3(localTarget.x, localTarget.y, -localTarget.z);

	const float dist1 = destPos.Distance(target);
	if (dist1 <= arg.target->radius)
		return 1; // destination

	if (UINT_MAX != arg.from)
	{
		const Vector3 target2 = frPos + localTarget.ToOpenGL();
		const float dist2 = destPos.Distance(target2);
		if (dist2 <= arg.target->radius)
			return 2; // destination from node
	}

	// inverse direction
	Quaternion rot2;
	rot2.SetRotationArc(f, -dir, Vector3(0, 1, 0));
	const Vector3 localTarget2 = local * rot2;
	const Vector3 target3 = toPos + localTarget2.ToOpenGL();

	const float dist3 = destPos.Distance(target3);
	if (dist3 <= arg.target->radius)
		return 1; // destination

	if (UINT_MAX != arg.from)
	{
		const Vector3 target4 = frPos + localTarget2.ToOpenGL();
		const float dist4 = destPos.Distance(target4);
		if (dist4 <= arg.target->radius)
			return 2; // destination from node
	}

	return 0;
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
	removevector2(m_vertices, vtxIdx);
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


// reserver vertex buffer
void cPathFinder2::ReservedVertexBuffer(const uint vertexCount)
{
	m_vertices.reserve(vertexCount);
}


void cPathFinder2::Clear()
{
	m_vertices.clear();
}
