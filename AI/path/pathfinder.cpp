
#include "stdafx.h"
#include "pathfinder.h"

using namespace ai;


cPathFinder::cPathFinder()
{
}

cPathFinder::~cPathFinder()
{
	Clear();
}


bool cPathFinder::Create(const int vertexCount)
{
	m_vertices.reserve(vertexCount);
	m_areas.reserve(32);
	return true;
}


bool cPathFinder::Read(const StrPath &fileName)
{
	Clear();

	std::ifstream ifs(fileName.c_str());
	RETV(!ifs.is_open(), false);

	int state = 0;
	sVertex vtx;

	char line[256];
	while (ifs.getline(line, sizeof(line)))
	{
		std::stringstream ss(line);
		
		char token[64];
		switch (state)
		{
		case 0:
		{
			ss >> token;
			if (!strcmp(token, "Vertex"))
			{
				vtx = sVertex();
				state = 1;
			}
		}
		break;

		case 1:
		{
			ss >> token;
			assert(!strcmp(token, "type"));
			ss >> vtx.type;
			state = 2;
		}
		break;

		case 2:
		{
			ss >> token;
			assert(!strcmp(token, "pos"));
			ss >> vtx.pos.x >> vtx.pos.y >> vtx.pos.z;
			state = 3;
		}
		break;

		case 3:
		{
			ss >> token;
			assert(!strcmp(token, "dir"));
			//ss >> vtx.dir.x >> vtx.dir.y >> vtx.dir.z;
			state = 4;
		}
		break;

		case 4:
		{
			ss >> token;

			// data1 ~ 4 parsing
			if (!strcmp(token, "data"))
			{
				int idx = 0;
				int cnt = 0;
				do {
					ss >> vtx.data[idx++];
					++cnt;
				} while (!ss.eof() && (cnt < ARRAYSIZE(vtx.data)));
			}
			else
			{
				assert(!strcmp(token, "edge"));
				int idx = 0;
				int cnt = 0;
				do {
					ss >> vtx.edge[idx++].to;
					++cnt;
				} while (!ss.eof() && (cnt < sVertex::MAX_EDGE));

				AddVertex(vtx);
			
				state = 0;
			}
		}
		break;
		}
	}

	return true;
}


// Write Format
// Vertex
// type 1
//	pos 1 1 1
//	dir 0 0 1
//	edge 0 1 2
bool cPathFinder::Write(const StrPath &fileName)
{
	using namespace std;

	ofstream ofs(fileName.c_str());
	RETV(!ofs.is_open(), false);

	for (auto &v : m_vertices)
	{
		ofs << "Vertex" << endl;
		ofs << "\ttype " << v.type << endl;
		ofs << "\tpos " << v.pos.x << " " << v.pos.y << " " << v.pos.z << endl;
		//ofs << "\tdir " << v.dir.x << " " << v.dir.y << " " << v.dir.z << endl;
		ofs << "\tdir " << 0 << " " << 0 << " " << 0 << endl;
		ofs << "\tedge ";
		for (int i = 0; i < sVertex::MAX_EDGE; ++i)
		{
			if (v.edge[i].to < 0)
				break;
			ofs << v.edge[i].to << " ";
		}
		ofs << endl;
	}

	return true;
}


// 길찾기
// start, end : 길찾기 시작, 종료 위치
//				sVertex상의 pos좌표 값이어야 한다.
// out : 길찾기가 성공하면, 최종 경로를 위치로 저장한다.
// outTrackVertexIndices: 길찾기가 성공하면, 최종 경로를 버텍스 인덱스로 저장한다.
// outTrackEdges : 길찾기가 성공하면, 최종 경로를 엣지로 저장한다.
// disableEdges: 길찾기에서 제외될 엣지
bool cPathFinder::Find(const Vector3 &start
	, const Vector3 &end
	, OUT vector<Vector3> &out
	, OUT vector<int> *outTrackVertexIndices //= NULL
	, OUT vector<sEdge> *outTrackEdges //= NULL
	, const set<sEdge> *disableEdges //= NULL
)
{
	const int startIdx = m_areas.empty() ? GetNearestVertex(start) : GetNearestVertex(start, end);
	if (startIdx < 0)
		return false;

	const int endIdx = m_areas.empty() ? GetNearestVertex(end) : GetNearestVertex(end, start);
	if (endIdx < 0)
		return false;

	set<int> visitSet;
	//map<int, float> lenSet; // key = edgeKey, data = length
	m_lenSet.clear();

	vector<int> candidate;
	candidate.reserve(m_vertices.size());
	candidate.push_back(startIdx);
	m_vertices[startIdx].startLen = 0;
	m_vertices[startIdx].endLen = Distance(start, end);

	int loopCount1 = 0; // debug, loop count
	int loopCount2 = 0; // debug, insertion count
	bool isFind = false;
	while (!candidate.empty())
	{
		const int curIdx = candidate.front();
		rotatepopvector(candidate, 0);

		sVertex &curVtx = m_vertices[curIdx];

		if (endIdx == curIdx)
		{
			isFind = true;
			break;
		}

		for (int i = 0; i < sVertex::MAX_EDGE; ++i)
		{
			if (curVtx.edge[i].to < 0)
				break;

			const int nextIdx = curVtx.edge[i].to;
			sVertex &nextVtx = m_vertices[nextIdx];

			const int edgeKey1 = MakeEdgeKey(curIdx, nextIdx);
			const int edgeKey2 = MakeEdgeKey(nextIdx, curIdx);
			if (visitSet.end() != visitSet.find(edgeKey1))
				continue; // is visit?

			if (disableEdges)
			{
				if (disableEdges->end() != disableEdges->find(sEdge(curIdx, nextIdx)))
					continue;
			}

			nextVtx.startLen = curVtx.startLen + Distance(curVtx.pos, nextVtx.pos) + 0.00001f;
			nextVtx.endLen = Distance(end, nextVtx.pos);

			visitSet.insert(edgeKey1);
			visitSet.insert(edgeKey2);
			m_lenSet[edgeKey1] = nextVtx.startLen + nextVtx.endLen;
			m_lenSet[edgeKey2] = nextVtx.startLen + nextVtx.endLen;

			// sorting candidate
			// value = minimum( startLen + endLen )
			bool isInsert = false;
			for (u_int k = 0; k < candidate.size(); ++k)
			{
				++loopCount1;

				sVertex &compVtx = m_vertices[candidate[k]];
				if ((compVtx.endLen + compVtx.startLen)
			> (nextVtx.endLen + nextVtx.startLen))
				{
					++loopCount2;

					candidate.push_back(nextIdx);
					common::rotateright2(candidate, k);
					isInsert = true;
					break;
				}
			}

			if (!isInsert)
				candidate.push_back(nextIdx);
		}
	}

	if (!isFind)
		return false;

	// backward tracking
	// end point to start point
	vector<int> verticesIndices;

	out.push_back(m_vertices[endIdx].pos);
	verticesIndices.push_back(endIdx);

	visitSet.clear();

	int curIdx = endIdx;
	while ((curIdx != startIdx) && (verticesIndices.size() < 1000))
	{
		float minEdge = FLT_MAX;
		int nextIdx = -1;
		sVertex &vtx = m_vertices[curIdx];
		for (int i = 0; i < sVertex::MAX_EDGE; ++i)
		{
			if (vtx.edge[i].to < 0)
				break;
			
			const int edgeKey = MakeEdgeKey(curIdx, vtx.edge[i].to);
			if (visitSet.end() != visitSet.find(edgeKey))
				continue; // is visit?

			auto it = m_lenSet.find(edgeKey);
			if (m_lenSet.end() == it)
				continue;

			if (minEdge > it->second)
			{
				minEdge = it->second;
				nextIdx = vtx.edge[i].to;
			}
		}

		if (nextIdx < 0)
		{
			assert(0);
			break; // error occur
		}

		visitSet.insert(MakeEdgeKey(curIdx, nextIdx));
		visitSet.insert(MakeEdgeKey(nextIdx, curIdx));
		out.push_back(m_vertices[nextIdx].pos);
		verticesIndices.push_back(nextIdx);
		curIdx = nextIdx;
	}

	assert(verticesIndices.size() < 1000);

	std::reverse(out.begin(), out.end());
	std::reverse(verticesIndices.begin(), verticesIndices.end());

	if (outTrackVertexIndices)
		*outTrackVertexIndices = verticesIndices;

	// Store Tracking Edge
	if (outTrackEdges)
	{
		outTrackEdges->reserve(verticesIndices.size());
		for (u_int i = 0; i < verticesIndices.size() - 1; ++i)
		{
			const int from = verticesIndices[i];
			const int to = verticesIndices[i + 1];
			outTrackEdges->push_back(sEdge(from, to));
		}
	}

	OptimizeAreaPath(start, end, out, verticesIndices);

	if (!out.empty() && (out.back() != end))
		out.push_back(end);

	return true;
}


void cPathFinder::OptimizeAreaPath(const Vector3 &start
	, const Vector3 &end
	, INOUT vector<Vector3> &out
	, INOUT vector<int> &verticesIndices
)
{
	// Optimize Start Area
	if (m_areas.empty())
		return;

	const int startAreaId = GetNearestArea(start);
	if (startAreaId >= 0)
	{
		int cnt = 0;
		bool isPathNode = false;
		for (u_int i = 0; i < out.size() - 1; ++i)
		{
			auto &p = out[i];
			if (!m_areas[startAreaId].rect.IsIn(p.x, p.z))
				break;

			if (1 == m_vertices[verticesIndices[i]].type)
				isPathNode = true;

			++cnt;
		}

		if (isPathNode)
			--cnt;

		for (int i = 0; i < cnt; ++i)
		{
			common::rotatepopvector(out, 0);
			common::rotatepopvector(verticesIndices, 0);
		}
	}

	// Optimize End Area
	const int endAreaId = GetNearestArea(end);
	if (endAreaId >= 0)
	{
		int cnt = 0;
		bool isPathNode = false;
		for (int i = (int)out.size() - 1; i >= 0; --i)
		{
			auto &p = out[i];
			if (!m_areas[endAreaId].rect.IsIn(p.x, p.z))
				break;

			if (1 == m_vertices[verticesIndices[i]].type)
				isPathNode = true;

			++cnt;
		}

		if (isPathNode)
			--cnt;

		for (int i = 0; i < cnt; ++i)
			out.pop_back();
	}

	// 같은 에어리어에서 움직일 때, path 버텍스 타입으로 인해 없어지지 않았던 버텍스를
	// 지운다. 같은 에어리어 내에서는 기준 버텍스 없이 움직일 수 있다.
	if ((startAreaId >= 0) && (startAreaId == endAreaId) && !out.empty())
		out.pop_back();
}


bool cPathFinder::AddVertex(const sVertex &vtx)
{
	m_vertices.push_back(vtx);
	return true;
}


bool cPathFinder::AddEdge(const int vtxIdx, const int addEdgeIdx)
{
	RETV2(vtxIdx < 0, false);
	RETV2((int)m_vertices.size() <= vtxIdx, false);
	RETV2(vtxIdx == addEdgeIdx, false);

	sVertex &vtx = m_vertices[vtxIdx];

	bool isAlreadyExist = false;
	for (int i = 0; i < sVertex::MAX_EDGE; ++i)
	{
		if (0 > vtx.edge[i].to)
			break;

		if (addEdgeIdx == vtx.edge[i].to)
		{
			isAlreadyExist = true;
			break;
		}
	}

	RETV(isAlreadyExist, false);

	// push back
	for (int i = 0; i < sVertex::MAX_EDGE; ++i)
	{
		if (0 > vtx.edge[i].to)
		{
			vtx.edge[i].to = addEdgeIdx;
			return true;
		}
	}

	return false;
}


// remove edge vtxIdx <-> removeEdgeIdx
bool cPathFinder::RemoveEdgeEachOther(const int vtxIdx, const int removeEdgeIdx)
{
	RemoveEdge(vtxIdx, removeEdgeIdx);
	RemoveEdge(removeEdgeIdx, vtxIdx);
	return true;
}


// remove edge vtxIdx -> removeEdgeIdx
bool cPathFinder::RemoveEdge(const int vtxIdx, const int removeEdgeIdx)
{
	RETV(vtxIdx < 0, false);
	RETV((int)m_vertices.size() <= vtxIdx, false);

	sVertex &vtx = m_vertices[vtxIdx];

	for (int i = 0; i < sVertex::MAX_EDGE; ++i)
	{
		if (0 > vtx.edge[i].to)
			break;

		if (removeEdgeIdx == vtx.edge[i].to)
		{
			for (int k = i; k < sVertex::MAX_EDGE - 1; ++k)
				vtx.edge[k].to = vtx.edge[k + 1].to;
			vtx.edge[sVertex::MAX_EDGE - 1].to = -1;
			return true;
		}
	}

	return false;
}


bool cPathFinder::RemoveVertex(const int index)
{
	// remove edge and decrease index if greater than remove index
	for (auto &v : m_vertices)
	{
		for (int i = 0; i < sVertex::MAX_EDGE; ++i)
		{
			if (0 > v.edge[i].to)
				break;

			if (index == v.edge[i].to) // rotate left
			{
				for (int k = i; k < sVertex::MAX_EDGE - 1; ++k)
				{
					if (v.edge[k].to < 0)
						break;
					v.edge[k].to = v.edge[k + 1].to;
				}

				v.edge[sVertex::MAX_EDGE - 1].to = -1;

				--i; // for loop bugfix
			}
			else if (index < v.edge[i].to)
			{
				--v.edge[i].to; // decrease index
			}
		}
	}

	// remove index vertex
	common::rotatepopvector(m_vertices, index);
	
	return true;
}


int cPathFinder::GetNearestVertex(const Vector3 &pos) const
{
	RETV(m_vertices.empty(), -1);
	
	int cnt = 0;
	int indices[sVertex::MAX_VERTEX];

	if (m_areas.empty())
	{
		for (u_int i = 0; i < m_vertices.size(); ++i)
			if (cnt < ARRAYSIZE(indices))
				indices[cnt++] = (int)i;
	}
	else
	{
		for (auto &area : m_areas)
		{
			if (area.rect.IsIn(pos.x, pos.z))
			{
				for (auto idx : area.indices)
					if (cnt < ARRAYSIZE(indices))
						indices[cnt++] = idx;
				break;
			}
		}
	}

	int idx = 0;
	float minLen = FLT_MAX;

	for (int i=0; i < cnt; ++i)
	{
		auto &vtx = m_vertices[ indices[i]];
		const float len = vtx.pos.LengthRoughly(pos);
		if (len < minLen)
		{
			idx = indices[i];
			minLen = len;
		}
	}

	return idx;
}


// return = minimum( distance(pos, nearest vertex) + distance(nearest vertex, end) )
int cPathFinder::GetNearestVertex(const Vector3 &pos, const Vector3 &end) const
{
	RETV(m_vertices.empty(), -1);

	int cnt = 0;
	int indices[sVertex::MAX_VERTEX];

	if (m_areas.empty())
	{
		for (u_int i = 0; i < m_vertices.size(); ++i)
			if (cnt < ARRAYSIZE(indices))
				indices[cnt++] = (int)i;
	}
	else
	{
		for (auto &area : m_areas)
		{
			if (area.rect.IsIn(pos.x, pos.z))
			{
				for (auto idx : area.indices)
					if (cnt < ARRAYSIZE(indices))
						indices[cnt++] = idx;
				break;
			}
		}
	}

	int idx = 0;
	float minLen = FLT_MAX;

	for (int i = 0; i < cnt; ++i)
	{
		auto &vtx = m_vertices[indices[i]];
		const float len = vtx.pos.LengthRoughly(pos) + vtx.pos.LengthRoughly(end);		
		if (len < minLen)
		{
			idx = indices[i];
			minLen = len;
		}
	}

	return idx;
}


int cPathFinder::GetNearestArea(const Vector3 &pos) const
{
	for (u_int i=0; i < m_areas.size(); ++i)
	{
		auto &area = m_areas[i];
		if (area.rect.IsIn(pos.x, pos.z))
			return i;
	}
	return -1;
}


// return vertex index from linkId == linkId
// return -1, not found
int cPathFinder::GetVertexFromLinkId(const int linkId) const
{
	for (u_int i = 0; i < m_vertices.size(); ++i)
		if (linkId == m_vertices[i].linkId)
			return (int)i;
	return -1;
}


// pos 와 가장 가까운 edge 를 리턴한다. 없으면 = -1 리턴
std::pair<int, int> cPathFinder::GetNearestEdge(const Vector3 &pos) const
{
	set<int> visitSet;

	int from = -1, to = -1;
	float minLen = FLT_MAX;
	for (int i = 0; i < (int)m_vertices.size(); ++i)
	{
		auto &vertex = m_vertices[i];
		for (int k = 0; k < sVertex::MAX_EDGE; ++k)
		{
			if (vertex.edge[k].to < 0)
				break;
			
			const int e = vertex.edge[k].to;
			const int edgeKey = MakeEdgeKey(i, e);
			if (visitSet.end() != visitSet.find(edgeKey))
				continue; // is visit?

			visitSet.insert(edgeKey);
			visitSet.insert(MakeEdgeKey(e, i));

			const float len = common::GetShortestLen(m_vertices[i].pos, m_vertices[e].pos, pos);
			if (len < minLen)
			{
				from = i;
				to = e;
				minLen = len;
			}
		}
	}

	return { from, to };
}


// Manhatan Distance
float cPathFinder::Distance_Manhatan(const Vector3 &p0, const Vector3 &p1) const
{
	return abs(p0.x - p1.x) + abs(p0.y - p1.y) + abs(p0.z - p1.z);
}


// Normal Distance
float cPathFinder::Distance(const Vector3 &p0, const Vector3 &p1) const
{
	return p0.Distance(p1);
}


// Add Area
bool cPathFinder::AddArea(const sRectf &area)
{
	// Check Already Exist
	for (auto &p : m_areas)
	{
		if (p.rect == area)
			return false;
	}
	
	sArea data;
	data.id = m_areas.size();
	data.rect = area;
	data.indices.reserve(16);

	// add vertex
	for (u_int i=0; i < m_vertices.size(); ++i)
	{
		sVertex &v = m_vertices[i];
		if (area.IsIn(v.pos.x, v.pos.z))
			data.indices.push_back(i);
	}

	m_areas.push_back(data);
	return true;
}


int cPathFinder::MakeEdgeKey(const int from, const int to)
{
	return from * 1000 + to;
}


void cPathFinder::Clear()
{
	m_vertices.clear();
}
