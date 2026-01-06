
#include "stdafx.h"
#include "pathfinder2.h"

using namespace ai;


cPathFinder2::cPathFinder2()
{
}
cPathFinder2::cPathFinder2(const cPathFinder2& rhs)
{
	operator=(rhs);
}
cPathFinder2::~cPathFinder2()
{
	Clear();
}


// read file (binary type)
//
// file format:
// vertex count
// vertex array
// edge count
// edge array
bool cPathFinder2::Read(const string& fileName)
{
	Clear();

	using namespace std;
	ifstream ifs(fileName, ios::binary);
	if (!ifs.is_open())
		return false; // error return

	char fmt[3];
	ifs.read(fmt, 3);
	if ((fmt[0] != 'P') || (fmt[1] != 'A') || (fmt[2] != 'T'))
		return false; // file format error

	int vtxSize = 0;
	ifs.read((char*)&vtxSize, sizeof(vtxSize));

	for (int i = 0; i < vtxSize; ++i)
	{
		sVertex vtx;
		ifs.read((char*)&vtx.id, sizeof(vtx.id));
		ifs.read((char*)&vtx.type, sizeof(vtx.type));
		ifs.read((char*)&vtx.pos, sizeof(vtx.pos));
		vtx.name.Format("%d", vtx.id);
		AddVertex(vtx);
	}

	int edgeSize = 0;
	ifs.read((char*)&edgeSize, sizeof(edgeSize));
	for (int i = 0; i < edgeSize; ++i)
	{
		int from, to;
		ifs.read((char*)&from, sizeof(from));
		ifs.read((char*)&to, sizeof(to));
		AddEdge(from, to);
	}
	return true;
}


// write file
bool cPathFinder2::Write(const string& fileName)
{
	using namespace std;
	ofstream ofs(fileName, ios::binary);
	if (!ofs.is_open())
		return false; // error return

	ofs.write("PAT", 3); // file extents

	int edgeSize = 0;
	const int vtxSize = (int)m_vertices.size();
	ofs.write((char*)&vtxSize, sizeof(vtxSize));
	for (auto& vtx : m_vertices)
	{
		ofs.write((char*)&vtx.id, sizeof(vtx.id));
		ofs.write((char*)&vtx.type, sizeof(vtx.type));
		ofs.write((char*)&vtx.pos, sizeof(vtx.pos));
		edgeSize += (int)vtx.edges.size();
	}

	ofs.write((char*)&edgeSize, sizeof(edgeSize));
	for (uint i = 0; i < m_vertices.size(); ++i)
	{
		auto& vtx = m_vertices[i];
		for (auto& e : vtx.edges)
		{
			ofs.write((char*)&i, sizeof(i));
			ofs.write((char*)&e.to, sizeof(e.to));
		}
	}
	return true;
}


// find path
bool cPathFinder2::Find(const Vector3 &start, const Vector3 &end
	, OUT vector<Vector3> &out
	, const set<sEdge2> *disableEdges //= nullptr
	, OUT vector<uint> *outTrackVertexIndices //= nullptr
	, OUT vector<sEdge2> *outTrackEdges //= nullptr
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
	, const set<sEdge2> *disableEdges //= nullptr
	, OUT vector<uint> *outTrackVertexIndices //= nullptr
	, OUT vector<sEdge2> *outTrackEdges //= nullptr
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
			outTrackEdges->push_back(sEdge2(from, to));
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
	, const set<sEdge2> *disableEdges //= nullptr
	, const bool isChangeDirPenalty //= false
	, const sTarget* target //= nullptr
	, const float rotationLimit //= -2.f
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
					for (int i = ((int)close.size() - 1); i >= 0; --i)
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
		for (auto &tr : vtx0.edges)
		{
			const auto it2 = cset.find(tr.to);
			if (cset.end() != it2)
				continue;

			const sVertex &vtx1 = m_vertices[tr.to];

			// calc direction change penalty
			float penalty = 0.f;
			if (UINT_MAX != node.prev)
			{
				const sVertex& vtx2 = m_vertices[node.prev];
				const Vector3 dir0 = (vtx0.pos - vtx2.pos).Normal();
				const Vector3 dir1 = (vtx1.pos - vtx0.pos).Normal();
				const float d = dir0.DotProduct(dir1);
				if (d < rotationLimit)
					continue; // block path
				if (isChangeDirPenalty && (d < 0.7f))
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
	sNode *node = &close.back();
	while (1)
	{
		out.push_back(node->idx);
		if (node->idx == startIdx)
			break; // finish
		auto it = find_if(close.begin(), close.end()
			, [&](auto &a) {return a.idx == node->prev; });
		if (it == close.end())
			break; // error occurred
		node = &*it;
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


// create edge map, direction map
bool cPathFinder2::CreateEdgeMap()
{
	m_edgeMap.clear();

	for (uint i = 0; i < m_vertices.size(); ++i)
		for (auto &edge : m_vertices[i].edges)
			m_edgeMap[{i, edge.to}] = &edge;
	return true;
}


// add vertex
// return vertex index
uint cPathFinder2::AddVertex(const sVertex &vtx)
{
	m_vertices.push_back(vtx);
	const uint idx = (uint)m_vertices.size() - 1;
	const int id = atoi(vtx.name.c_str());
	m_vertices[idx].id = id;
	return idx;
}


// add edge
// prop: edge property
bool cPathFinder2::AddEdge(const uint fromVtxIdx, const uint toVtxIdx
	, const int prop //=0
	, const float speed //= 0.f
)
{
	RETV2((int)m_vertices.size() <= fromVtxIdx, false);
	RETV(fromVtxIdx == toVtxIdx, false);

	sVertex &vtx = m_vertices[fromVtxIdx];
	if (IsExistEdge(fromVtxIdx, toVtxIdx))
		return false; // already linked

	sEdge edge;
	edge.to = toVtxIdx;
	edge.distance = 1.f;
	edge.w = 1.f;
	edge.speed = speed;
	edge.prop = prop;
	edge.toWaypoint = 0;
	edge.enable = true;
	edge.isCurve = false; // line type
	edge.curveAngle = 0.f;
	edge.curveDist = 0.f;
	edge.dirFrVtxIdx = -1;
	vtx.edges.push_back(edge);
	return true;
}


// set cuve edge
// angle: 0 ~ 180 degree
// directionFrVtxIdx: direction from vertex index (to determine curve direction)
bool cPathFinder2::SetCurveEdge(const uint fromVtxIdx, const uint toVtxIdx
	, const bool isCurve, const float angle, const float dist, const float arcLen
	, const uint directionFrVtxIdx )
{
	RETV2((int)m_vertices.size() <= fromVtxIdx, false);
	RETV2((int)m_vertices.size() <= toVtxIdx, false);
	RETV(fromVtxIdx == toVtxIdx, false);

	sVertex& vtx = m_vertices[fromVtxIdx];
	for (auto &edge : vtx.edges) 
	{
		if (edge.to == toVtxIdx)
		{
			edge.isCurve = isCurve;
			if (isCurve)
			{
				edge.curveAngle = angle;
				edge.curveDist = dist;
				edge.distance = arcLen;
				edge.dirFrVtxIdx = directionFrVtxIdx;
			}
			break;
		}
	}
	return true;
}


// is exist same edge?
bool cPathFinder2::IsExistEdge(const uint fromVtxIdx, const uint toVtxIdx)
{
	RETV2((int)m_vertices.size() <= fromVtxIdx, false);
	RETV(fromVtxIdx == toVtxIdx, false);
	sVertex &vtx = m_vertices[fromVtxIdx];
	for (auto &tr : vtx.edges)
		if (tr.to == toVtxIdx)
			return true;
	return false;
}


// remove edge
bool cPathFinder2::RemoveEdge(const uint fromVtxIdx, const uint toVtxIdx)
{
	RETV((int)m_vertices.size() <= fromVtxIdx, false);
	sVertex &vtx = m_vertices[fromVtxIdx];
	int idx = -1;
	for (uint i = 0; i < vtx.edges.size(); ++i)
	{
		if (vtx.edges[i].to == toVtxIdx)
		{
			idx = (int)i;
			break;
		}
	}
	if (idx < 0)
		return false; // not found edge
	common::popvector(vtx.edges, idx);
	return true;
}


// remove vertex
bool cPathFinder2::RemoveVertex(const uint vtxIdx)
{
	// remove edge and decrease index if greater than remove index
	for (auto &v : m_vertices)
		for (uint i=0; i < v.edges.size(); ++i)
			if (vtxIdx == v.edges[i].to)
			{
				common::popvector(v.edges, i);
				break;
			}
	for (auto &v : m_vertices)
		for (auto &tr : v.edges)
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


// return edge from, to
cPathFinder2::sEdge* cPathFinder2::GetEdge(const uint from, const uint to)
{
	if ((m_vertices.size() >= from) || (m_vertices.size() >= to))
		return nullptr;
	sVertex &vtx = m_vertices[from];
	for (auto& edge : vtx.edges)
		if (edge.to == to)
			return &edge;
	return nullptr;
}


// reserver vertex buffer
void cPathFinder2::ReservedVertexBuffer(const uint vertexCount)
{
	m_vertices.reserve(vertexCount);
}


// return bezier curve edge remain distance
// edge: bezier curve edge
// pos: position on the bezier curve
// ratio:	0.0 ~ 1.0, (move distance ratio)
//			-1.0: no on the bezier curve
float cPathFinder2::GetCurveEdgeDistance(const sEdge& edge, const Vector3& pos
	, OUT float *ratio //=nullptr
)
{
	if (edge.bezier.empty() || (edge.bezier.size() == 1))
	{
		if (edge.ctrlPts.empty())
			return 0.f;
		else
			return edge.ctrlPts.back().Distance(pos);
	}

	float minDist = FLT_MAX;
	uint idx = 0; // min distance index
	for (uint i = 0; i < edge.bezier.size() - 1; ++i)
	{
		const Vector3 &p0 = edge.bezier[i];
		const Vector3 &p1 = edge.bezier[i + 1];

		const Line line(p0, p1);
		const float d = line.GetDistance(pos);
		if (d < minDist)
		{
			idx = i;
			minDist = d;
		}
	}

	const Vector3 &v1 = edge.bezier[idx + 1];
	const float dist1 = v1.Distance(pos);
	const float dist2 = edge.bezierLens.back() - edge.bezierLens[idx + 1];
	float d = dist1 + dist2;

	if (ratio)
	{
		const float totLen = edge.bezierLens.back();
		if (totLen <= 0.f)
		{
			// exception process
			*ratio = -1.f;
			return 0.f;
		}
		else
		{
			if ((idx != 0) && (dist1 > 0.15f))
			{
				// exception process
				*ratio = -1.f;
				return d;
			}
			*ratio = max(0.f, 1.f - (d / totLen));
		}
	}

	return d;
}


// return next position on bezier curve path
// edge: bezier curve edge
// pos: current position
// dist: current position moving distance
// outPos: next moving pos on the bezier curve
// outDir: next direction on the bezier curve
// outRatio: 0.0 ~ 1.0, curve position 0:start, 1:end
// return: success?
bool cPathFinder2::GetCurveEdgeMove(const sEdge& edge, const Vector3& pos
	, const float dist
	, OUT Vector3& outPos
	, OUT Vector3& outDir
	, OUT float *outRatio //=nullptr
)
{
	if (edge.bezier.empty() || edge.ctrlPts.empty())
		return false;
	if (edge.bezier.size() == 1)
		return false;

	float minDist = FLT_MAX;
	uint idx = 0; // min distance index
	for (uint i = 0; i < edge.bezier.size() - 1; ++i)
	{
		const Vector3& p0 = edge.bezier[i];
		const Vector3& p1 = edge.bezier[i + 1];

		const Line line(p0, p1);
		const float d = line.GetDistance(pos);
		if (d < minDist)
		{
			idx = i;
			minDist = d;
		}
	}

	float newDist = dist;
	Vector3 newPos = pos;
	if (0 == idx)
	{
		const Vector3& p0 = edge.bezier[idx];
		const Vector3& p1 = edge.bezier[idx + 1];
		const Vector3 curPos(pos.x, p0.y, pos.z);
		const float d0 = p0.Distance(curPos);
		const float d1 = p1.Distance(curPos);
		Vector3 dir = (p1 - p0).Normal();
		Vector3 dir0 = (p0 - curPos).Normal();
		Vector3 dir1 = (p1 - curPos).Normal();
		if (dir0.IsEmpty())
			dir0 = dir;
		if (dir1.IsEmpty())
			dir1 = dir;

		if (dir0.DotProduct(dir1) < 0.f)
		{
			// nothing to do
		}
		else
		{
			//if (d0 < d1)
			//{
			//	if (d0 > dist)
			//	{
			//		outPos = pos + dir0 * dist;
			//		//outDir = dir0;
			//		outDir = dir;
			//		return true; // finish
			//	}
			//	else
			//	{
			//		newDist = dist - d0;
			//		newPos = Vector3(p0.x, pos.y, p0.z);
			//	}
			//}
			//else
			//{
				if (d1 < dist)
				{
					outPos = pos + dir1 * dist;
					//outDir = dir1;
					outDir = dir;
					return true; // finish
				}
				else
				{
					newDist = dist - d1;
					newPos = Vector3(p1.x, pos.y, p1.z);
				}
			//}
		}
	}
	else if ((edge.bezier.size() - 2) == idx)
	{
		const Vector3& p0 = edge.bezier[idx];
		const Vector3& p1 = edge.bezier[idx + 1];
		const Vector3 curPos(pos.x, p0.y, pos.z);
		const float d0 = p0.Distance(curPos);
		const float d1 = p1.Distance(curPos);
		const Vector3 dir = (p1 - p0).Normal();
		const Vector3 dir0 = (p0 - curPos).Normal();
		const Vector3 dir1 = (p1 - curPos).Normal();

		if (dir0.DotProduct(dir1) < 0.f)
		{
			// nothing to do
		}
		else
		{
			//if (d0 < d1)
			//{
			//	if (d0 > dist)
			//	{
			//		outPos = pos + dir0 * dist;
			//		//outDir = dir0;
			//		outDir = dir;
			//		return true; // finish
			//	}
			//	else
			//	{
			//		newDist = dist - d0;
			//		newPos = Vector3(p0.x, pos.y, p0.z);
			//	}
			//}
			//else
			//{
				if (d1 < dist)
				{
					outPos = pos + dir1 * dist;
					//outDir = dir1;
					outDir = dir;
					return true; // finish
				}
				else
				{
					newDist = dist - d1;
					newPos = Vector3(p1.x, pos.y, p1.z);
				}
			//}
		}
	}

	float ratio = 0.f;
	{
		const Vector3& p0 = edge.bezier[idx];
		const Vector3& p1 = edge.bezier[idx + 1];
		const float d1 = p0.Distance(newPos);
		const float d2 = p1.Distance(newPos);
		if (d1 + d2 == 0.f)
			return false;
		ratio = d1 / (d1 + d2);
	}

	const float lineDist = edge.bezierLens[idx + 1] - edge.bezierLens[idx];
	const float len1 = lineDist * ratio;
	const float moveLen = edge.bezierLens[idx] + len1 + newDist;

	uint moveIdx = (uint)edge.bezierLens.size() - 1;
	for (uint i = idx + 1; i < edge.bezierLens.size(); ++i)
	{
		if (edge.bezierLens[i] > moveLen)
		{
			moveIdx = i;
			break;
		}
	}

	{
		const float offsetLen = moveLen - edge.bezierLens[moveIdx - 1];
		const Vector3 &p0 = edge.bezier[moveIdx - 1];
		const Vector3 &p1 = edge.bezier[moveIdx];
		const Vector3 dir = (Vector3(p1.x,0.f,p1.z) - Vector3(p0.x,0.f,p0.z)).Normal();
		outPos = p0 + dir * offsetLen;
		outPos.y = newPos.y; // ignore y-axis
		outDir = dir;
		if (outRatio)
			*outRatio = (float)moveIdx / (float)edge.bezierLens.size();
	}

	return true;
}


// return nearest curve position, direction
// edge: bezier curve edge
// pos: current position
// dir: current direction
// dist: current position moving distance
// outPos: next moving pos on the bezier curve
// outDir: next direction on the bezier curve
// return: success?
bool cPathFinder2::GetCurveEdgeFirstPos(const sEdge& edge
	, const Vector3& pos
	, const Vector3& dir
	, const float dist
	, OUT Vector3& outPos
	, OUT Vector3& outDir)
{
	if (edge.bezier.empty() || edge.ctrlPts.empty())
		return false;
	if (edge.bezier.size() == 1)
		return false;

	float maxDir = -1.f;
	float minDist = FLT_MAX;
	uint idx = 0; // min distance index
	for (uint i = 0; i < edge.bezier.size() - 1; ++i)
	{
		const Vector3& p0 = edge.bezier[i];
		const Vector3& p1 = edge.bezier[i + 1];

		const Line line(p0, p1);
		const float d = line.dir.DotProduct(dir);
		if (maxDir < d)
		{
			idx = i;
			maxDir = d;
			minDist = line.GetDistance(pos);
		}
	}

	const Vector3& p0 = edge.bezier[idx];
	const Vector3& p1 = edge.bezier[idx + 1];
	Vector3 center = (p0 + p1) / 2.f;
	center.y = 0;
	const float d0 = center.Distance(Vector3(pos.x, 0.f, pos.z));

	if (dist < d0)
	{
		const Vector3 dir0 = (center - Vector3(pos.x, 0.f, pos.z)).Normal();
		outPos = pos + dir0 * dist;
		outDir = dir0;
		return true;
	}
	else
	{
		return GetCurveEdgeMove(edge, center, dist - d0, outPos, outDir);
	}
}


// is on the curve line?
bool cPathFinder2::IsOnEdge(const sEdge& edge, const Vector3& pos
	, const float offset //= 0.05f
)
{
	if (edge.bezier.empty() || edge.ctrlPts.empty())
		return false;
	if (edge.bezier.size() == 1)
		return false;
	for (uint i = 0; i < edge.bezier.size() - 1; ++i)
	{
		const Vector3& p0 = edge.bezier[i];
		const Vector3& p1 = edge.bezier[i + 1];
		const Line line(p0, p1);
		const float d = line.GetDistance(Vector3(pos.x, p0.y, pos.z));
		if (d <= offset)
			return true;
	}
	return false;
}


// serialize path data (same as file read/write)
bool cPathFinder2::Serialize(OUT vector<BYTE>& out)
{
	// calc bytesize 
	int byteSize = 0;
	const int vtxSize = (int)m_vertices.size();
	byteSize += sizeof(int); // vertex size
	byteSize += m_vertices.size() * 20;
	int edgeSize = 0;
	for (auto& vtx : m_vertices)
		edgeSize += (int)vtx.edges.size();
	byteSize += sizeof(int); // edge size
	byteSize += edgeSize * 8;
	//byteSize += sizeof(int); // bbox size
	//byteSize += m_bounds.size() * 44;
	//~

	out.clear();
	out.resize(byteSize, 0);
	int cursor = 0;
	BYTE* dst = &out[0];

	*(int*)(dst + cursor) = (int)m_vertices.size();
	cursor += sizeof(int);
	for (auto& vtx : m_vertices)
	{
		*(int*)(dst + cursor) = vtx.id;
		cursor += sizeof(vtx.id);

		*(int*)(dst + cursor) = vtx.type;
		cursor += sizeof(vtx.type);

		*(Vector3*)(dst + cursor) = vtx.pos;
		cursor += sizeof(vtx.pos);
	}

	*(int*)(dst + cursor) = edgeSize;
	cursor += sizeof(edgeSize);
	for (uint i = 0; i < (uint)m_vertices.size(); ++i)
	{
		auto& vtx = m_vertices[i];
		for (auto& e : vtx.edges)
		{
			*(int*)(dst + cursor) = i;
			cursor += sizeof(i);

			*(int*)(dst + cursor) = e.to;
			cursor += sizeof(e.to);
		}
	}

	//*(int*)(dst + cursor) = (int)m_bounds.size();
	//cursor += sizeof(int);
	//for (auto& bound : m_bounds)
	//{
	//	*(int*)(dst + cursor) = bound.type;
	//	cursor += sizeof(bound.type);

	//	*(Vector3*)(dst + cursor) = *(Vector3*)&bound.bbox.m_bbox.Center;
	//	cursor += sizeof(Vector3);

	//	*(Vector3*)(dst + cursor) = *(Vector3*)&bound.bbox.m_bbox.Extents;
	//	cursor += sizeof(Vector3);

	//	*(Quaternion*)(dst + cursor) = *(Quaternion*)&bound.bbox.m_bbox.Orientation;
	//	cursor += sizeof(Quaternion);
	//}

	return true;
}


// assign operator
cPathFinder2& cPathFinder2::operator=(const cPathFinder2& rhs)
{
	if (this != &rhs)
	{
		m_vertices = rhs.m_vertices;
		
		m_edgeMap.clear();
		for (uint i = 0; i < (uint)m_vertices.size(); ++i)
		{
			auto& vtx = m_vertices[i];
			for (auto& e : vtx.edges)
				m_edgeMap.insert({ std::make_pair(i, e.to), &e });
		}
	}
	return *this;
}


void cPathFinder2::Clear()
{
	m_vertices.clear();
}
