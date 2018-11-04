
#include "stdafx.h"
#include "pathfinder.h"
#include "pathfinder2d.h"


namespace ai
{
	enum { MAX_VERTEX = 1024};
	float g_edges_len2[MAX_VERTEX][MAX_VERTEX];
	bool  g_edges_visit2[MAX_VERTEX][MAX_VERTEX];
}

using namespace ai;
cPathFinder2D::sVertex cPathFinder2D::m_dummy;



cPathFinder2D::cPathFinder2D()
	: m_map(NULL)
	, m_graph(NULL)
{
}

cPathFinder2D::~cPathFinder2D()
{
	Clear();
}


bool cPathFinder2D::Read(const char *fileName)
{
	auto size = GetRowsCols(fileName);
	if (size.first <= 0)
		return false;

	Clear();

	using namespace std;
	ifstream ifs(fileName);
	if (!ifs.is_open())
		return false;

	m_rows = size.first;
	m_cols = size.second;
	m_map = new sVertex[size.first * size.second];
	ZeroMemory(m_map, size.first * size.second);

	int y = 0;
	string line;
	while (getline(ifs, line))
	{
		if (line.empty())
			break;

		int x = 0;
		for (u_int i = 0; i < line.size(); ++i)
		{
			int n = line[i] - '0';
			if (abs(n) > 9)
				return false;

			GetMap(y * m_cols + x).type = (BYTE)n;
			++x;
		}
		++y;
	}

	// find waypoint
	m_waypoints.clear();
	for (int i = 0; i < m_rows; ++i)
		for (int k = 0; k < m_cols; ++k)
			if (3 == GetMap(i*m_cols + k).type)
				m_waypoints.push_back(Vector2i(k,i));

	GenerateGraphNode();

	return true;
}


// m_map 정보를 이용해서 Graph를 생성한다.
bool cPathFinder2D::GenerateGraphNode()
{
	// map 노드에서 좌우,위아래에서 연결된 노드가 3개 이상일 경우,
	// waypoint로 인식하고, 그래프의 노드가 된다.
	// 노드는 항상 양방향 노드로 처리한다.

	m_dummy = sVertex(); // 혹시 모를 버그를 막기위해 초기화

	if (!m_graph)
		m_graph = new cPathFinder();
	m_graph->Clear();

	map<Vector2i, int> waypoints; // pos, vertex index

	vector<Vector2i> q;
	q.push_back(Vector2i(0, 0));

	while (!q.empty())
	{
		const Vector2i curPos = q.back();
		q.pop_back();

		auto it = waypoints.find(curPos);
		const int curVertexIdx = (waypoints.end() == it) ? -1 : it->second;

		// 4방향으로 이동하면서 waypoint와 만나는지 확인한다.
		const Vector2i offsetPos[] = { Vector2i(-1,0), Vector2i(0,-1), Vector2i(1,0), Vector2i(0,1) };
		for (int i = 0; i < ARRAYSIZE(offsetPos); ++i)
		{
			Vector2i nextPos = curPos + offsetPos[i];
		
			while (CheckRange(nextPos))
			{
				// check edge?
				// 진행 방향으로 90도 꺽은 방향으로 노드가 있으면 waypoint다.
				const Vector2i orthoEdge = offsetPos[(i + 1) % ARRAYSIZE(offsetPos)];
				if ( (CheckRange(nextPos + orthoEdge) && (0 != GetMap(nextPos + orthoEdge).type))
					|| (CheckRange(nextPos - orthoEdge) && (0 != GetMap(nextPos - orthoEdge).type)))
				{
					// waypoint를 발견했다면, 이미 등록된 waypoint인지 확인한다.
					if (waypoints.end() == waypoints.find(nextPos))
					{
						q.push_back(nextPos);

						// add vertex
						cPathFinder::sVertex vtx;
						vtx.type = 0;
						vtx.pos = Vector3((float)nextPos.x, 0, (float)nextPos.y);
						m_graph->AddVertex(vtx);

						const int addVertexIdx = (int)m_graph->m_vertices.size() - 1;
						waypoints[nextPos] = addVertexIdx;

						// link vertex
						if (curVertexIdx >= 0)
						{
							m_graph->AddEdge(curVertexIdx, addVertexIdx);
							m_graph->AddEdge(addVertexIdx, curVertexIdx);
						}
					}
					else
					{
						// link vertex
						if (curVertexIdx >= 0)
						{
							const int linkVertexIdx = waypoints.find(nextPos)->second;
							m_graph->AddEdge(curVertexIdx, linkVertexIdx);
							m_graph->AddEdge(linkVertexIdx, curVertexIdx);
						}
					}

					break; // meet waypoint? finish loop
				}

				nextPos += offsetPos[i];
			}
		}
	}

	return true;
}


// pos가 map에 접근할 수 있는 값이면 true를 리턴한다.
inline bool cPathFinder2D::CheckRange(const Vector2i &pos)
{
	if ((pos.x < 0)
		|| (pos.x >= m_cols)
		|| (pos.y < 0)
		|| (pos.y >= m_rows))
		return false;
	return true;
}


inline cPathFinder2D::sVertex& cPathFinder2D::GetMap(const int idx)
{
	if ((idx < 0) || (idx >= (m_rows * m_cols)))
		return m_dummy;
	if (!m_map)
		return m_dummy;
	return m_map[idx];
}


inline cPathFinder2D::sVertex& cPathFinder2D::GetMap(const Vector2i &pos)
{
	return GetMap(pos.y*m_cols + pos.x);
}


// a-star path finder
bool cPathFinder2D::Find(const Vector2i &startPos
	, const Vector2i &endPos
	, OUT vector<Vector2i> &out)
{
	ZeroMemory(g_edges_visit2, sizeof(g_edges_visit2));
	ZeroMemory(g_edges_len2, sizeof(g_edges_len2));

	vector<Vector2i> candidate;
	candidate.reserve(m_rows * m_cols);
	candidate.push_back(startPos);

	const int startIdx = startPos.y*m_cols + startPos.x;
	const int endIdx = endPos.y*m_cols + endPos.x;
	GetMap(startIdx).startLen = 0;
	GetMap(startIdx).endLen = Distance_Manhatan(startPos, endPos);

	int loopCount1 = 0; // debug, loop count
	int loopCount2 = 0; // debug, insertion count
	bool isFind = false;
	while (!candidate.empty())
	{
		const Vector2i curPos = candidate.front();
		rotatepopvector(candidate, 0);

		const int curIdx = curPos.y*m_cols + curPos.x;
		sVertex &curVtx = GetMap(curIdx);

		if (endPos == curPos)
		{
			isFind = true;
			break;
		}

		const Vector2i offsetPos[] = { Vector2i(-1,0), Vector2i(0,-1), Vector2i(1,0), Vector2i(0,1) };
		for (int i = 0; i < ARRAYSIZE(offsetPos); ++i)
		{
			const Vector2i nextPos = curPos + offsetPos[i];
			if ((nextPos.x < 0)
				|| (nextPos.x >= m_cols)
				|| (nextPos.y < 0)
				|| (nextPos.y >= m_rows))
				continue;

			const int nextIdx = nextPos.y*m_cols + nextPos.x;
			sVertex &nextVtx = GetMap(nextIdx);
			if ((0 == nextVtx.type) // wall node
				|| (4 == nextVtx.type)) // block waypoint node
				continue; // ignore this node

			if (g_edges_visit2[curIdx][nextIdx])
				continue;

			float alpha = 0.f;
			if (1 == nextVtx.type)
				alpha = 1.1f;

			nextVtx.startLen = curVtx.startLen + Distance_Manhatan(curPos, nextPos) + alpha + 0.00001f;
			nextVtx.endLen = Distance_Manhatan(endPos, nextPos);
			g_edges_visit2[curIdx][nextIdx] = true;
			g_edges_visit2[nextIdx][curIdx] = true;
			g_edges_len2[curIdx][nextIdx] = nextVtx.startLen + nextVtx.endLen;
			g_edges_len2[nextIdx][curIdx] = nextVtx.startLen + nextVtx.endLen;

			// sorting candidate
			// value = minimum( startLen + endLen )
			bool isInsert = false;
			for (u_int k = 0; k < candidate.size(); ++k)
			{
				++loopCount1;

				sVertex &compVtx = GetMap(candidate[k].y * m_cols + candidate[k].x);
				if ((compVtx.endLen + compVtx.startLen)
					> (nextVtx.endLen + nextVtx.startLen))
				{
					++loopCount2;

					candidate.push_back(nextPos);
					common::rotateright2(candidate, k);
					isInsert = true;
					break;
				}
			}

			if (!isInsert)
				candidate.push_back(nextPos);
		}
	}

	if (!isFind)
		return false;

	// tracking end point to start point
	vector<Vector2i> verticesIndices;

	out.push_back(endPos);
	verticesIndices.push_back(endPos);

	ZeroMemory(g_edges_visit2, sizeof(g_edges_visit2));

	Vector2i curPos = endPos;
	while ((curPos != startPos) && (verticesIndices.size() < 1000))
	{
		float minEdge = FLT_MAX;
		Vector2i nextPos(-1,-1);

		const int curIdx = curPos.y*m_cols + curPos.x;
		sVertex &vtx = GetMap(curIdx);
		
		const Vector2i offsetPos[] = { Vector2i(-1,0), Vector2i(0,-1), Vector2i(1,0), Vector2i(0,1) };
		for (int i = 0; i < ARRAYSIZE(offsetPos); ++i)
		{
			const Vector2i edgePos = curPos + offsetPos[i];
			if ((edgePos.x < 0)
				|| (edgePos.x >= m_cols)
				|| (edgePos.y < 0)
				|| (edgePos.y >= m_rows))
				continue;

			const int edgeIdx = edgePos.y*m_cols + edgePos.x;
			if (g_edges_visit2[curIdx][edgeIdx])
				continue;

			const float len = g_edges_len2[curIdx][edgeIdx];
			if (0 == len)
				continue;

			if (minEdge > len)
			{
				minEdge = len;
				nextPos = edgePos;
			}
		}

		if (nextPos.x < 0)
		{
			assert(0);
			return false; // error occur
		}

		const int nextIdx = nextPos.y*m_cols + nextPos.x;

		g_edges_visit2[curIdx][nextIdx] = true;
		g_edges_visit2[nextIdx][curIdx] = true;
		out.push_back(nextPos);
		verticesIndices.push_back(nextPos);
		curPos = nextPos;
	}

	assert(verticesIndices.size() < 1000);

	std::reverse(out.begin(), out.end());
	std::reverse(verticesIndices.begin(), verticesIndices.end());

	return true;
}


// 길찾기 열거
// 모든 가능한 길을 열거한다.
// WayPoint가 중복일 때는 제외한다.
bool cPathFinder2D::FindEnumeration(const Vector2i &startPos
	, const Vector2i &endPos
	, OUT vector<vector<Vector2i>> &out)
{
	RETV(!m_graph, false);

	Vector3 p0((float)startPos.x, 0, (float)startPos.y);
	Vector3 p1((float)endPos.x, 0, (float)endPos.y);

	vector<vector<int>> totWps;
	vector<int> wpsSet;
	vector<std::pair<int, int>> blockWps; // from -> to vertex index
	int combination = 0; // waypoint combination

	while (0)
	{
		// find path
		vector<Vector3> path;
		vector<int> wayPoints;
		if (!m_graph->Find(p0, p1, path, &wayPoints))
			continue;

		// search already stored waypoint list
		{
			auto it = std::find(totWps.begin(), totWps.end(), wayPoints);
			if (totWps.end() != it)
				continue; // if find, ignore this path
		}

		totWps.push_back(wayPoints);

		for (auto &wp : wayPoints)
		{
			if (wpsSet.end() == std::find(wpsSet.begin(), wpsSet.end(), wp))
				wpsSet.push_back(wp);
		}

		// recovery waypoint on/off
		for (auto &idx : blockWps)
			m_graph->AddEdge(idx.first, idx.second);
		blockWps.clear();

		// next waypoint on/off combination
		if ((int)wpsSet.size() <= combination)
			continue;

		++combination;

		//for (u_int i = 0; i < wpsSet.size(); ++i)
		//{
		//	const int f = combination >> i;
		//	if (0x01 & f)
		//	{
		//		// block waypoint
		//		auto &from = m_graph->m_vertices[wpsSet[i]];
		//		for (int k = 0; k < cPathFinder::sVertex::MAX_EDGE; ++k)
		//		{
		//			if (0 > from.edge[k])
		//				continue;

		//			auto &to = m_graph->m_vertices[from.edge[k]];


		//		}
		//	}
		//}


		//vector<Vector2i> path;
		//if (!Find(startPos, endPos, path))
		//	continue; // not found path

		// find waypoint
		//set<Vector2i> wps;
		//for (auto &wp : m_waypoints)
		//{
		//	auto it = std::find(path.begin(), path.end(), wp);
		//	if (path.end() != it)
		//		wps.insert(*it);
		//}

		// clear block waypoint
		//for (auto &wp : m_waypoints)
		//	GetMap(wp).type = 3;

		// search already stored waypoint list
		//{
		//	auto it = std::find(totWps.begin(), totWps.end(), wps);
		//	if (totWps.end() != it)
		//		continue; // if find, ignore this path

		//	totWps.push_back(wps);
		//}
	}

	//if (wps.empty())
	//	return true;

	//GetMap(*wps.begin()).type = 4;

	//vector<Vector2i> path2;
	//if (!Find(startPos, endPos, path2))
	//	return false;

	return true;
}


// Manhatan Distance
float cPathFinder2D::Distance_Manhatan(const Vector2i &p0, const Vector2i &p1) const
{
	return (float)(abs(p0.x - p1.x) + abs(p0.y - p1.y));
}


// return map file rows, cols size
// 1 character size
std::pair<int, int> cPathFinder2D::GetRowsCols(const char *fileName)
{
	using namespace std;
	ifstream ifs(fileName);
	if (!ifs.is_open())
		return { 0, 0 };

	int rows = 0;
	int cols = 0;

	string line;
	while (getline(ifs, line))
	{
		if (line.empty())
			break;
		cols = max((int)line.size(), cols);
		++rows;
	}

	return { rows, cols };
}


void cPathFinder2D::Clear()
{
	SAFE_DELETEA(m_map);
	SAFE_DELETE(m_graph);
}
