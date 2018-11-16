
#include "stdafx.h"
#include "pathfinder.h"
#include "pathfinder2d.h"


using namespace ai;
cPathFinder2D::sVertex cPathFinder2D::m_dummy;



cPathFinder2D::cPathFinder2D()
	: m_map(NULL)
	, m_fastmap(NULL)
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

	if (!m_fastmap)
		m_fastmap = new cPathFinder();
	m_fastmap->Clear();

	set<Vector2i> visitSet;
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

			if (visitSet.end() != visitSet.find(nextPos))
				continue; // already visit
			visitSet.insert(nextPos);

			while (CheckRange(nextPos))
			{
				// check edge?
				// 진행 방향으로 90도 꺽은 방향으로 노드가 있으면 waypoint다.
				// offsetPos[]가 시계방향으로 저장되기 때문에 +1은 항상 90도 꺽은 방향이다.
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
						m_fastmap->AddVertex(vtx);

						const int addVertexIdx = (int)m_fastmap->m_vertices.size() - 1;
						waypoints[nextPos] = addVertexIdx;

						// link vertex
						if (curVertexIdx >= 0)
						{
							m_fastmap->AddEdge(curVertexIdx, addVertexIdx);
							m_fastmap->AddEdge(addVertexIdx, curVertexIdx);

							// move backward to update edgekey
							GetMap(curPos).edgeKey = -1; // waypoint hasn't edgeKey
							Vector2i back = nextPos - offsetPos[i];
							while (curPos != back)
							{
								GetMap(back).edgeKey = MakeUniqueEdgeKey(curVertexIdx, addVertexIdx);
								back -= offsetPos[i];
							}
						}
					}
					else
					{
						// link vertex
						if (curVertexIdx >= 0)
						{
							const int linkVertexIdx = waypoints.find(nextPos)->second;
							m_fastmap->AddEdge(curVertexIdx, linkVertexIdx);
							m_fastmap->AddEdge(linkVertexIdx, curVertexIdx);

							// move backward to update edgekey
							GetMap(curPos).edgeKey = -1; // waypoint hasn't edgeKey
							Vector2i back = nextPos - offsetPos[i];
							while (curPos != back)
							{
								GetMap(back).edgeKey = MakeUniqueEdgeKey(curVertexIdx, linkVertexIdx);
								back -= offsetPos[i];
							}
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
	set<int> visitSet;
	map<int, float> lenSet; // key = edgeKey, data = length

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

			const int edgeKey1 = MakeEdgeKey(curIdx, nextIdx);
			const int edgeKey2 = MakeEdgeKey(nextIdx, curIdx);
			if (visitSet.end() != visitSet.find(edgeKey1))
				continue; // is visit?

			float alpha = 0.f;
			if (1 == nextVtx.type)
				alpha = 1.1f;

			nextVtx.startLen = curVtx.startLen + Distance_Manhatan(curPos, nextPos) + alpha + 0.00001f;
			nextVtx.endLen = Distance_Manhatan(endPos, nextPos);
			visitSet.insert(edgeKey1);
			visitSet.insert(edgeKey2);
			lenSet[edgeKey1] = nextVtx.startLen + nextVtx.endLen;
			lenSet[edgeKey2] = nextVtx.startLen + nextVtx.endLen;

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

	visitSet.clear();

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
			const int edgeKey = MakeEdgeKey(curIdx, edgeIdx);
			if (visitSet.end() != visitSet.find(edgeKey))
				continue; // is visit?

			auto it = lenSet.find(edgeKey);
			if (lenSet.end() == it)
				continue;
			const float len = it->second;
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

		visitSet.insert(MakeEdgeKey(curIdx, nextIdx));
		visitSet.insert(MakeEdgeKey(nextIdx, curIdx));
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
// Edge가 중복일 때는 제외한다.
bool cPathFinder2D::FindEnumeration(const Vector2i &startPos
	, const Vector2i &endPos
	, OUT vector<ppath> &outPos
	, OUT vector<ppath> &outVertexPos
	, OUT vector<epath> &outEdges
)
{
	RETV(!m_fastmap, false);

	const Vector3 p0((float)startPos.x, 0, (float)startPos.y);
	const Vector3 p1((float)endPos.x, 0, (float)endPos.y);

	vector<epath> allEpath; // total edge path
	epath edgeSet;
	set<cPathFinder::sEdge> disableEdges;
	int combination = 0; // disable edge combination
	
	// 다양한 경로를 생성하기위해, 특정 엣지를 막는다.
	// 전체 경로의 앞부분 과, 뒷부분 몇개를 조합해서, 막는다.
	// front
	const int comboSize = 3;
	int combinationIndices1[][comboSize] = {
		{0, -1}
		,{ 0, -1}
		,{ 0, -1}
		,{ 1, -1}
		,{ 1, -1}
		,{ 1, -1}
		,{ 2, -1 }
		,{ 2, -1 }
		,{ 2, -1 }
		,{ 0, 2, -1 }
		,{ 0, 2, -1 }
		,{ 0, 2, -1 }
	};

	// back (path.size() - offset)
	int combinationIndices2[][comboSize] = {
		{ -1 }
		,{ 1, -1 }
		,{ 2, -1 }
		,{ -1 }
		,{ 1, -1 }
		,{ 2, -1 }
		,{ -1 }
		,{ 1, -1 }
		,{ 2, -1 }
		,{ -1 }
		,{ 1, -1 }
		,{ 2, -1 }
	};

	// start, end node 를 추가한다.
	AddTemporaryVertexAndEdges(p0);
	AddTemporaryVertexAndEdges(p1);

	while (combination < 12) 
	{
		// find path
		vector<Vector3> vpath;
		vector<int> wayPoints;
		epath edges;
		if (m_fastmap->Find(p0, p1, vpath, &wayPoints, &edges, &disableEdges))
		{
			// search already stored path list
			{
				auto it = std::find(allEpath.begin(), allEpath.end(), edges);
				if (allEpath.end() != it)
					goto next; // if find, ignore this path
			}

			allEpath.push_back(edges);

			// add all path edges
			for (auto &e : edges)
			{
				if (edgeSet.end() == std::find(edgeSet.begin(), edgeSet.end(), e))
					edgeSet.push_back(e);
			}

			// next edge on/off combination
			if ((int)edgeSet.size() <= combination)
				continue;
		}

	next:
		// composite disable edge 
		disableEdges.clear();

		// front path skip
		for (int i = 0; i < comboSize; ++i)
		{
			const int idx = combinationIndices1[combination][i];
			if (idx < 0)
				break;
			if (idx < (int)edgeSet.size())
				disableEdges.insert(edgeSet[idx]);
		}

		// back path skip
		for (int i = 0; i < comboSize; ++i)
		{
			const int idx = (int)edgeSet.size() - combinationIndices2[combination][i];
			if ((idx < 0) || ((int)edgeSet.size() <= idx))
				break;
			if (idx > comboSize)
				disableEdges.insert(edgeSet[idx]);
		}

		++combination;
	}

	// Store Enumeration Path
	outPos.reserve(allEpath.size());
	outVertexPos.reserve(allEpath.size());
	outEdges.reserve(allEpath.size());
	for (u_int i = 0; i < allEpath.size(); ++i)
	{
		if (allEpath[i].empty())
			continue;

		outEdges.push_back(allEpath[i]);

		outPos.push_back({});
		GetEdgePath2PosPath(allEpath[i], outPos[i]);

		outVertexPos.push_back({});
		outVertexPos[i].reserve(allEpath[i].size());
		for (u_int k = 0; k < allEpath[i].size(); ++k)
		{
			const int from = allEpath[i][k].from;
			outVertexPos[i].push_back(
				Vector2i((int)m_fastmap->m_vertices[from].pos.x
					, (int)m_fastmap->m_vertices[from].pos.z));
		}

		const int to = allEpath[i].back().to;
		outVertexPos[i].push_back(
			Vector2i((int)m_fastmap->m_vertices[to].pos.x
				, (int)m_fastmap->m_vertices[to].pos.z));
	}

	RemoveTemporaryVertexAndEdges(p0);
	RemoveTemporaryVertexAndEdges(p1);

	return true;
}


// m_fastmap의 edge path 정보로 position path 를 생성해서 리턴한다.
// 첫번째 버텍스와 마지막 버텍스는 동적으로 추가된 버텍스이기 때문에, sVertex::edgeKey가 
// 존재하지 않는다. 그래서 replaceFromIdx 를 참조해서 검색한다.
// 나머지 버텍스도 비슷하지만, edgeKey가 존재하기 때문에, 조금 더 간단하다.
bool cPathFinder2D::GetEdgePath2PosPath(const epath &edgePath, OUT ppath &out)
{
	RETV(!m_fastmap, false);
	RETV(edgePath.empty(), false);

	// front vertex
	// 거꾸로 검색한 후, 뒤집는다.
	// 시작 버텍스가 어느방향으로 가야할지 모르기 때문임.
	bool isFrontReverse = false;

	if (edgePath.size() == 1)
	{
		const cPathFinder::sEdge &front = edgePath.front();
		const auto &frontV = m_fastmap->m_vertices[front.from];
		const auto &backV = m_fastmap->m_vertices[front.to];

		// 추가된 버텍스인지 아닌지에 따라 collect 되는 방식이 다르다.
		const bool b1 = (frontV.replaceFromIdx >= 0);
		const bool b2 = (backV.replaceFromIdx >= 0);

		// 1. 한 엣지 안에 start, end가 있을 경우
		if (b1 && b2)
		{
			CollectEdgeAddedVertices(frontV.replaceFromIdx, frontV.replaceToIdx
				, MakeUniqueEdgeKey(frontV.replaceFromIdx, frontV.replaceToIdx)
				, front.from, front.to
				, out);
		}
		// 2. from만 추가된 버텍스 일 경우
		else if (b1 && !b2)
		{
			// 거꾸로 검색한 뒤, 뒤집는다.
			isFrontReverse = true;
		}
		// 3. to만 추가된 버텍스 일 경우
		else if (!b1 && b2)
		{
			const int to = (front.from == backV.replaceFromIdx) ? backV.replaceToIdx : backV.replaceFromIdx;
			CollectEdgeVertices(front.from, front.to, MakeUniqueEdgeKey(front.from, to), out);

			const auto &toV = m_fastmap->m_vertices[front.to];
			out.push_back(Vector2i((int)toV.pos.x, (int)toV.pos.z));
		}
		// 4. 둘다 추가된 버텍스가 아닐 경우
		else if (!b1 && !b2)
		{
			CollectEdgeVertices(front.from, front.to, MakeUniqueEdgeKey(front.to, front.from), out);

			const auto &toV = m_fastmap->m_vertices[front.to];
			out.push_back(Vector2i((int)toV.pos.x, (int)toV.pos.z));
		}
		else
		{
			assert(0);
		}
	}

	if (isFrontReverse || (edgePath.size() > 1))
	{
		const cPathFinder::sEdge &front = edgePath.front();
		const auto &frontV = m_fastmap->m_vertices[front.from];

		int from = front.from;
		if (frontV.replaceFromIdx >= 0) // temporary added vertex?
			from = (frontV.replaceToIdx == front.to) ? frontV.replaceFromIdx : frontV.replaceToIdx;

		CollectEdgeVertices(front.to, front.from, MakeUniqueEdgeKey(front.to, from), out);

		out.push_back(Vector2i((int)frontV.pos.x, (int)frontV.pos.z));
		std::reverse(out.begin(), out.end());
		
		if (!isFrontReverse) // 엣지 리스트가 2개 이상일 때만 해당된다.
			out.pop_back(); // 마지막 노드는 중복이기 때문에 제거
	}

	for (u_int i = 1; i < edgePath.size() - 1; ++i)
	{
		const cPathFinder::sEdge &edge = edgePath[i];
		CollectEdgeVertices(edge.from, edge.to, MakeUniqueEdgeKey(edge.to, edge.from), out);
	}

	// back vertex
	if (edgePath.size() > 1)
	{
		// 마지막 버텍스가 어느방향으로 가야할지 모르기 때문에 
		// replaceFromIdx, replaceToIdx를 참조한다.
		const cPathFinder::sEdge &back = edgePath.back();
		const auto &backV = m_fastmap->m_vertices[back.to];

		int to = back.to;
		if (backV.replaceFromIdx >= 0) // temporary added vertex?
			to = (backV.replaceFromIdx == back.from) ? backV.replaceToIdx : backV.replaceFromIdx;

		CollectEdgeVertices(back.from, back.to, MakeUniqueEdgeKey(back.from, to), out);
		out.push_back(Vector2i((int)backV.pos.x, (int)backV.pos.z));
	}

	return true;
}


// fastmap의 엣지 정보를 토대로, from에서 to로 향하는 버텍스 위치 정보를
// out에 저장해서 리턴한다.
bool cPathFinder2D::CollectEdgeVertices(const int from, const int to
	, const int uniqueEdgeKey, OUT ppath &out)
{
	auto &fromV = m_fastmap->m_vertices[from];
	const auto &destV = m_fastmap->m_vertices[to];
	const Vector2i destPos = Vector2i((int)destV.pos.x, (int)destV.pos.z);

	const Vector2i curPos = Vector2i((int)fromV.pos.x, (int)fromV.pos.z);
	const Vector2i offsetPos[] = { Vector2i(-1,0), Vector2i(0,-1), Vector2i(1,0), Vector2i(0,1) };
	for (int i = 0; i < ARRAYSIZE(offsetPos); ++i)
	{
		const Vector2i nextPos = curPos + offsetPos[i];
		if (!CheckRange(nextPos))
			continue;

		if (uniqueEdgeKey == GetMap(nextPos).edgeKey)
		{
			// to waypoint로 향하는 버텍스 발견
			// to 버텍스에 도착할 때까지 위치 저장
			Vector2i p = curPos;
			while (CheckRange(p) && (destPos != p))
			{
				out.push_back(p);
				p += offsetPos[i];
			}
			
			return true;
		}
	}

	return false;
}


// fastmap의 엣지 정보를 토대로, from에서 to로 향하는 버텍스 위치 정보를
// out에 저장해서 리턴한다.
// 이 때, 추가된 버텍스는 edgekey가 설정되어 있지 않으므로, from에서 to로 향하는 엣지 중에 
// 추가된 버텍스가 나타나면 out에 저장하는 방식으로 처리한다. 
// 한 엣지에 추가된 버텍스가 2개가 있을 때 처리하는 함수다.
bool cPathFinder2D::CollectEdgeAddedVertices(const int from, const int to, const int uniqueEdgeKey
	, const int addedVertexFrom, const int addedVertexTo
	, OUT ppath &out)
{
	const auto &fromV = m_fastmap->m_vertices[from];
	const auto &destV = m_fastmap->m_vertices[to];
	const Vector2i destPos = Vector2i((int)destV.pos.x, (int)destV.pos.z);

	const auto &addedFromV = m_fastmap->m_vertices[addedVertexFrom];
	const auto &addedToV = m_fastmap->m_vertices[addedVertexTo];
	const Vector2i addedFromPos = Vector2i((int)addedFromV.pos.x, (int)addedFromV.pos.z);
	const Vector2i addedToPos = Vector2i((int)addedToV.pos.x, (int)addedToV.pos.z);

	const Vector2i curPos = Vector2i((int)fromV.pos.x, (int)fromV.pos.z);
	const Vector2i offsetPos[] = { Vector2i(-1,0), Vector2i(0,-1), Vector2i(1,0), Vector2i(0,1) };
	for (int i = 0; i < ARRAYSIZE(offsetPos); ++i)
	{
		const Vector2i nextPos = curPos + offsetPos[i];
		if (!CheckRange(nextPos))
			continue;

		if (uniqueEdgeKey == GetMap(nextPos).edgeKey)
		{
			// to waypoint로 향하는 버텍스 발견
			// to 버텍스에 도착할 때까지 위치 저장
			int state = 1;
			Vector2i p = curPos;
			while (CheckRange(p) && (state != 4))
			{
				switch (state)
				{
				case 1:
					if (p == addedFromPos)
					{
						out.push_back(p);
						state = 2;
					}
					else if (p == addedToPos)
					{
						out.push_back(p);
						state = 3;
					}
					break;

				case 2:
					out.push_back(p);
					if (addedToPos == p)
						state = 4; // finisth
					break;

				case 3:
					out.push_back(p);
					if (addedFromPos == p)
						state = 4; // finisth
					break;
				}

				p += offsetPos[i];
			}

			return true;
		}
	}

	return false;
}


// 임시 노드를 추가한다.
// 현재 위치가 버텍스 위에 있다면, Pos 버텍스를 추가하지 않는다.
// 이미 있기 때문에~
// 노드에 없는 위치에 Pos가 있다면, 새 버텍스를 추가한다.
bool cPathFinder2D::AddTemporaryVertexAndEdges(const Vector3 &pos)
{
	RETV(!m_fastmap, false);

	const int nearVtx = m_fastmap->GetNearestVertex(pos);
	if (nearVtx < 0)
		return true;

	if (pos == m_fastmap->m_vertices[nearVtx].pos)
		return true;

	// 노드를 추가하기 전에, 추가될 노드 주위의 엣지를 제거하고
	// 재연결한다.
	std::pair<int,int> edge = m_fastmap->GetNearestEdge(pos);
	if (edge.first < 0)
		return false; // error occur

	const int from = edge.first;
	const int to = edge.second;
	m_fastmap->RemoveEdge(from, to);
	m_fastmap->RemoveEdge(to, from);

	cPathFinder::sVertex vertex;
	vertex.type = 3; // temporary node
	vertex.replaceFromIdx = from;
	vertex.replaceToIdx = to;
	vertex.pos = pos;
	m_fastmap->AddVertex(vertex);
	const int addVtxIdx = m_fastmap->m_vertices.size() - 1;

	if (!m_fastmap->AddEdge(addVtxIdx, to))
	{
		assert(0);
	}
	if (!m_fastmap->AddEdge(to, addVtxIdx))
	{
		assert(0);
	}
	if (!m_fastmap->AddEdge(addVtxIdx, from))
	{
		assert(0);
	}
	if (!m_fastmap->AddEdge(from, addVtxIdx))
	{
		assert(0);
	}

	return true;
}


// 임시 노드로 추가한 것을 제거하고, 기존 graph를 복구한다.
bool cPathFinder2D::RemoveTemporaryVertexAndEdges(const Vector3 &pos)
{
	RETV(!m_fastmap, false);

	const int temporaryVtxIdx = m_fastmap->GetNearestVertex(pos);
	if (temporaryVtxIdx < 0)
		return true;

	// temporary node?
	cPathFinder::sVertex &tvertex = m_fastmap->m_vertices[temporaryVtxIdx];
	if (tvertex.type != 3)
		return true;

	// 노드를 제거하기 전에, 노드 주변 노드를 연결한다.
	int state = 0;
	int from, to;
	for (int i = 0; i < cPathFinder::sVertex::MAX_EDGE; ++i)
	{
		if (tvertex.edge[i] < 0)
			break;
		if (0 == state)
		{
			from = tvertex.edge[i];
			state = 1;
		}
		else if (1 == state)
		{
			to = tvertex.edge[i];
			state = 2;
		}
		else
		{
			assert(0);
		}
	}

	if (state != 2)
		return false;

	// edge 복구, 순서 중요
	m_fastmap->AddEdge(from, to);
	m_fastmap->AddEdge(to, from);
	m_fastmap->RemoveVertex(temporaryVtxIdx);

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


inline int cPathFinder2D::MakeEdgeKey(const int from, const int to) const
{
	return from * 1000 + to;
}


// 자기자신으로 향하는 노드는 생성할 수 없다.
inline int cPathFinder2D::MakeUniqueEdgeKey(const int from, const int to) const
{
	return max(from, to)*1000 + min(from, to);
}


// edgeKey 분리
inline std::pair<int, int> cPathFinder2D::SeparateEdgeKey(const int edgeKey) const
{
	return {edgeKey/1000, edgeKey%1000};
}


void cPathFinder2D::Clear()
{
	SAFE_DELETEA(m_map);
	SAFE_DELETE(m_fastmap);
}
