
#include "stdafx.h"
#include "navigationmesh.h"

using namespace ai;


cNavigationMesh::cNavigationMesh()
{
}

cNavigationMesh::~cNavigationMesh()
{
}


// pathFinder 파일을 읽어서, Navigation Mesh를 생성한다.
bool cNavigationMesh::ReadFromPathFile(const char *fileName)
{
	cPathFinder pathFinder;
	if (!pathFinder.Read(fileName))
		return false;

	m_vertices.clear();
	m_vertices.reserve(pathFinder.m_vertices.size());

	vector<WORD> indices;
	indices.reserve(pathFinder.m_vertices.size()*3);

	for (auto &vtx : pathFinder.m_vertices)
		m_vertices.push_back(vtx.pos);

	for (u_int i = 0; i < pathFinder.m_vertices.size(); ++i)
	{
		auto &vtx = pathFinder.m_vertices[i];
		if (vtx.edge[0] < 0)
			continue;

		// 첫 번째 Polygon을 찾아서, Navigation Mesh를 생성한다.
		BuildPolygonFromPathFinder(pathFinder, i, vtx.edge[0], -1, indices);
		break;
	}

	if (indices.empty())
		return false;

	return true;
}


// 두개의 버텍스가 포함된 폴리곤을 찾아서, 저장한다.
// 재귀적으로 다음 폴리곤을 검색해서 저장한다.
// 생성한 폴리곤 Index 를 리턴한다.
int cNavigationMesh::BuildPolygonFromPathFinder(const ai::cPathFinder &pathFinder
	, const int vtxIdx1, const int vtxIdx2, const int vtxIdx3
	, INOUT vector<WORD> &indices )
{
	bool isFind = false;
	const ai::sVertex vtx1 = pathFinder.m_vertices[vtxIdx1];
	const ai::sVertex vtx2 = pathFinder.m_vertices[vtxIdx2];

	// vtxIdx1, vtxIdx2 두개와 연결된 버텍스를 찾는다.
	// 서로 같은 버텍스를 가르키면, 하나의 폴리곤에 있음을 뜻한다.
	for (int i = 0; i < ai::sVertex::MAX_EDGE; ++i)
	{
		if (vtx1.edge[i] < 0)
			break;

		for (int k = 0; k < ai::sVertex::MAX_EDGE; ++k)
		{
			if (vtx2.edge[k] < 0)
				break;

			if (vtx1.edge[i] == vtx2.edge[k])
			{
				const int nextIdx3 = vtx1.edge[i];
				if (vtxIdx3 == nextIdx3)
					continue; // 상위단에서 이미 생성한 버텍스다.

				const int triIdx = GetExistTriangle(indices, vtxIdx1, vtxIdx2, nextIdx3);
				if (triIdx >= 0)
				{
					// vtxIdx1, vtxIdx2를 공통으로 하고, vtxIdx3 버텍스와 반대편에 있는
					// 노드를 뜻한다. 이미 만들어진 노드이니, node index를 리턴하고 종료한다.
					return triIdx / 3;
				}

				// vtxIdx1, vtxIdx2가 포함된 폴리곤 나머지 버텍스를 찾음.
				// 왼손 법칙을 이용해 Normal이 Vector3(0,1,0)이 되는 순서로 저장한다.
				const Vector3 p1 = vtx1.pos;
				const Vector3 p2 = vtx2.pos;
				const Vector3 p3 = pathFinder.m_vertices[nextIdx3].pos;
				Triangle tri(p1, p2, p3);
				if (tri.Normal().y > 0.f)
				{
					indices.push_back(vtxIdx1);
					indices.push_back(vtxIdx2);
					indices.push_back(nextIdx3);
				}
				else
				{
					indices.push_back(vtxIdx1);
					indices.push_back(nextIdx3);
					indices.push_back(vtxIdx2);
				}

				isFind = true;
				break;
			}
		}

		if (isFind)
			break;
	}

	if (!isFind)
		return -1; // 폴리곤 없음.

	// 위에서 찾은 폴리곤, 인덱스
	const int polyIndices[3] = {
		indices[indices.size() - 3]
		, indices[indices.size() - 2]
		, indices[indices.size() - 1]
	};

	// 생성한 네비게이션 메쉬 노드 인덱스를 리턴한다.
	sNaviNode node;
	node.idx1 = polyIndices[0];
	node.idx2 = polyIndices[1];
	node.idx3 = polyIndices[2];
	node.center = pathFinder.m_vertices[polyIndices[0]].pos;
	node.center += pathFinder.m_vertices[polyIndices[1]].pos;
	node.center += pathFinder.m_vertices[polyIndices[2]].pos;
	node.center /= 3.f;
	m_naviNodes.push_back(node);
	const int nodeId = m_naviNodes.size() - 1;

	// 인접한 폴리곤을 찾아서 저장한다.
	const int idx1 = BuildPolygonFromPathFinder(pathFinder, polyIndices[0], polyIndices[1], polyIndices[2], indices);
	const int idx2 = BuildPolygonFromPathFinder(pathFinder, polyIndices[1], polyIndices[2], polyIndices[0], indices);
	const int idx3 = BuildPolygonFromPathFinder(pathFinder, polyIndices[2], polyIndices[0], polyIndices[1], indices);

	m_naviNodes[nodeId].adjacent[0] = idx1;
	m_naviNodes[nodeId].adjacent[1] = idx2;
	m_naviNodes[nodeId].adjacent[2] = idx3;

	return nodeId;
}


// 등록된 폴리곤이 있는지 검색한다. 있다면 index 를 리턴한다.
// 없다면, -1을 리턴한다.
int cNavigationMesh::GetExistTriangle(const vector<WORD> &indices
	, const int vtxIdx1, const int vtxIdx2, const int vtxIdx3)
{
	for (u_int i = 2; i < indices.size(); i += 3)
	{
		const WORD idx1 = indices[i - 2];
		const WORD idx2 = indices[i - 1];
		const WORD idx3 = indices[i - 0];

		if (((idx1 == vtxIdx1) && (idx2 == vtxIdx2) && (idx3 == vtxIdx3))
			|| ((idx1 == vtxIdx1) && (idx2 == vtxIdx3) && (idx3 == vtxIdx2))
			|| ((idx1 == vtxIdx2) && (idx2 == vtxIdx1) && (idx3 == vtxIdx3))
			|| ((idx1 == vtxIdx2) && (idx2 == vtxIdx3) && (idx3 == vtxIdx1))
			|| ((idx1 == vtxIdx3) && (idx2 == vtxIdx1) && (idx3 == vtxIdx2))
			|| ((idx1 == vtxIdx3) && (idx2 == vtxIdx2) && (idx3 == vtxIdx1))
			)
			return (i-2);
	}
	return -1;
}

// Normal Distance
float Distance(const Vector3 &p0, const Vector3 &p1)
{
	return p0.Distance(p1);
}


// Search Navigation Mesh
bool cNavigationMesh::Find(const Vector3 &start, const Vector3 &end
	, OUT vector<Vector3> &out1
	, OUT vector<Vector3> &out2 
)
{
	const int startIdx = GetNearestNode(start);
	const int endIdx = GetNearestNode(end);

	if ((startIdx == -1) || (endIdx == -1))
		return false;

	ZeroMemory(g_edges_visit, sizeof(g_edges_visit));
	ZeroMemory(g_edges_len, sizeof(g_edges_len));

	vector<int> candidate;
	candidate.reserve(m_vertices.size());
	candidate.push_back(startIdx);

	m_naviNodes[startIdx].startLen = 0;
	m_naviNodes[startIdx].endLen = Distance(start, end);

	bool isFind = false;
	while (!candidate.empty())
	{
		const int curIdx = candidate.front();
		rotatepopvector(candidate, 0);

		sNaviNode &curNode = m_naviNodes[curIdx];

		if (endIdx == curIdx)
		{
			isFind = true;
			break;
		}

		for (int i = 0; i < 3; ++i)
		{
			if (curNode.adjacent[i] < 0)
				continue;

			const int nextIdx = curNode.adjacent[i];

			if (g_edges_visit[curIdx][nextIdx])
				continue;

			sNaviNode &nextNode = m_naviNodes[nextIdx];
			
			nextNode.startLen = curNode.startLen + Distance(curNode.center, nextNode.center) + 0.00001f;
			nextNode.endLen = Distance(end, nextNode.center);
			g_edges_visit[curIdx][nextIdx] = true;
			g_edges_visit[nextIdx][curIdx] = true;
			g_edges_len[curIdx][nextIdx] = nextNode.startLen + nextNode.endLen;
			g_edges_len[nextIdx][curIdx] = nextNode.startLen + nextNode.endLen;

			// sorting candidate
			// value = minimum( startLen + endLen )
			bool isInsert = false;
			for (u_int k = 0; k < candidate.size(); ++k)
			{
				sNaviNode &compNode = m_naviNodes[candidate[k]];
				if ((compNode.endLen + compNode.startLen) > (nextNode.endLen + nextNode.startLen))
				{
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

	// tracking end point to start point
	vector<int> nodeIndices;

	out1.push_back(m_naviNodes[endIdx].center);
	nodeIndices.push_back(endIdx);

	ZeroMemory(g_edges_visit, sizeof(g_edges_visit));

	int curIdx = endIdx;
	while ((curIdx != startIdx) && (nodeIndices.size() < 1000))
	{
		float minEdge = FLT_MAX;
		int nextIdx = -1;
		sNaviNode &node = m_naviNodes[curIdx];

		for (int i = 0; i < 3; ++i)
		{
			if (node.adjacent[i] < 0)
				continue;
			if (g_edges_visit[curIdx][node.adjacent[i]])
				continue;

			const float len = g_edges_len[curIdx][node.adjacent[i]];
			if (0 == len)
				continue;

			if (minEdge > len)
			{
				minEdge = len;
				nextIdx = node.adjacent[i];
			}
		}

		if (nextIdx < 0)
		{
			assert(0);
			break; // error occur
		}

		g_edges_visit[curIdx][nextIdx] = true;
		g_edges_visit[nextIdx][curIdx] = true;
		out1.push_back(m_naviNodes[nextIdx].center);
		nodeIndices.push_back(nextIdx);
		curIdx = nextIdx;
	}

	assert(nodeIndices.size() < 1000);

	out1.push_back(start);
	std::reverse(out1.begin(), out1.end());
	std::reverse(nodeIndices.begin(), nodeIndices.end());

	out1.push_back(end);

	OptimizePath(nodeIndices, out1, out2);

	return true;
}


// Find the node nearest to pos and return the index m_naviNodes.
// If NavigationMesh is created correctly, there are no duplicated nodes in pos.
// If not found, return -1
int cNavigationMesh::GetNearestNode(const Vector3 &pos)
{
	float nearDist = FLT_MAX;
	int nearIdx = -1;

	for (u_int i=0; i < m_naviNodes.size(); ++i)
	{
		auto &node = m_naviNodes[i];

		const Triangle tri(
			m_vertices[node.idx1]
			, m_vertices[node.idx2]
			, m_vertices[node.idx3]
		);

		if (tri.Intersect(Vector3(pos.x, pos.y + 1, pos.z), Vector3(0, -1, 0)))
			return (int)i;

		const float len = node.center.LengthRoughly(pos);
		if (len < nearDist)
		{
			nearDist = len;
			nearIdx = i;
		}
	}
	
	return nearIdx;
}


// Optimize the path found in Find ().
// 다음 노드까지 직진했을 때, 충돌이 일어나지 않으면, 그 다음 노드까지
// 같은 방식으로 검사해서, 최대한 직선으로 움직이게 한다.
// 두 노드 사이 붙어있는 인접면은 검사하지 않고, 인접하지 않는 면과
// 충돌이 일어났는지 판단한다.
void cNavigationMesh::OptimizePath(const vector<int> &nodeIndices
	, const vector<Vector3> &path
	, OUT vector<Vector3> &out)
{
	if (nodeIndices.empty() || (path.size() < 3))
		return;

	out.push_back(path[0]);

	Vector3 curPos = path[0] + Vector3(0, 1, 0); // start pos
	int curVtxIdx = -1; // 선회 Vertex Index
	int curVtxEdge1 = -1; // 선회면 Vertex Index1-2
	int curVtxEdge2 = -1;
	Triangle turnTri1, turnTri2; // 선회면

	Vector3 nextLocs[3];
	int nextLocsCnt = 0;

	int s = 0; // 첫번째 노드부터 검사
	for (int i = 0; i < (int)nodeIndices.size(); ++i)
	{
		//sNaviNode &curNode = m_naviNodes[curNodeIdx];
		//sNaviNode &nextNode = m_naviNodes[nextNodeIdx];

		// 마지막 노드일 때, 최종 도착지를 기준으로 검사한다.
		//Vector3 nextPos = ((nodeIndices.size()-2) == i)? 
		//	path.back() : nextNode.center;
		//nextPos += Vector3(0, 1, 0); // for collision

		//Vector3 nextPos;
		if ((nodeIndices.size() - 1) == i)
		{
			//nextPos = path.back();
			nextLocs[0] = path.back();
			nextLocsCnt = 1;
		}
		else
		{
			// 두 노드 인접면의 중점
			const int curNodeIdx = nodeIndices[i];
			const int nextNodeIdx = nodeIndices[i + 1];
			std::pair<int,int> adjIdx = GetAdjacentVertexIdx(curNodeIdx, nextNodeIdx);
			//nextPos = (m_vertices[adjIdx.first] + m_vertices[adjIdx.second]) * 0.5f;

			nextLocs[0] = m_vertices[adjIdx.first].Interpolate(m_vertices[adjIdx.second], 0.1f);
			nextLocs[1] = m_vertices[adjIdx.first].Interpolate(m_vertices[adjIdx.second], 0.5f);
			nextLocs[2] = m_vertices[adjIdx.first].Interpolate(m_vertices[adjIdx.second], 0.9f);
			nextLocsCnt = 3;
		}
		//nextPos += Vector3(0, 1, 0); // for Triangle Collision

		// Node의 변 중에, 인접하지 않는 변을, 통과한다면, 이동할 수 없는
		// 경로이기 때문에, 그 방향으로는 이동해서는 안된다.
		//const Vector3 dir = (nextPos - curPos).Normal();

		// 전체 경로에서 인접변 충돌 테스트
		for (int o = s; o < i; ++o)
		{
			const int preTestNodeIdx = nodeIndices[o];
			const int nextTestNodeIdx = nodeIndices[o+1];
			sNaviNode &testNode = m_naviNodes[nextTestNodeIdx];
			const int idxs[3] = { testNode.idx1, testNode.idx2, testNode.idx3 };

			// 인접변을 제외한 충돌 테스트
			for (int k = 0; k < 3; ++k)
			{
				if (testNode.adjacent[k] >= 0)
					continue;

				// Vertex를 중심으로 선회할 때, 같은 Vertex Index를 포함하면,
				// Vertex 위치를 dir 방향으로 조금 이동한다, 그렇지 않으면 버텍스가 
				// 면에 붙어있기 때문에, 항상 충돌 판정이 일어난다.
				const Vector3 offsetY(0, 10, 0);
				const Vector3 p1 = m_vertices[idxs[k]];
				const Vector3 p2 = m_vertices[idxs[(k + 1) % 3]];
				const Vector3 p3 = p1 + offsetY;
				const Triangle tri1(p1, p2, p3);
				const Triangle tri2(p1 + offsetY, p2, p2 + offsetY);

				// 인접하지 않는 변과 충돌 (갈 수 없는 길을 통과할 경우)
				bool collision1 = false;
				bool collision2 = false;
				int collIdx = -1;
				for (int p = 0; p < nextLocsCnt; ++p)
				{
					collIdx = p;
					Vector3 nextPos = nextLocs[p] + Vector3(0, 1, 0);
					const Vector3 dir = (nextPos - curPos).Normal();

					float t, u;
					collision1 = tri1.Intersect(curPos, dir, &t, &u)
								|| tri2.Intersect(curPos, dir, &t, &u);

					//bool collision2 = false;
					if ((curVtxEdge1 >= 0) && (curVtxEdge2 >= 0))
					{
						// 면의 뒤쪽에 있을 경우, nextPos와 가까운 버텍스가 선회포인트가 되어야 한다.
						Plane plane(turnTri1.a, turnTri1.b, turnTri1.c);
						if (plane.Distance(nextPos) < 0)
						{
							if (nextPos.LengthRoughly(m_vertices[curVtxEdge1]) 
								> nextPos.LengthRoughly(m_vertices[curVtxEdge2]))
							{
								collision2 = true;
							}
						}
						else
						{
							collision2 = turnTri1.Intersect(nextPos, -dir, &t, &u)
								|| turnTri2.Intersect(nextPos, -dir, &t, &u);
						}
					}
					else
					{
						collision2 = tri1.Intersect(nextPos, -dir, &t, &u)
							|| tri2.Intersect(nextPos, -dir, &t, &u);

						if (collision2)
						{
							curVtxEdge1 = idxs[k];
							curVtxEdge2 = idxs[(k + 1) % 3];
							turnTri1 = tri1;
							turnTri2 = tri2;
						}
					}

					if (collision1 || collision2)
						break;
				}

				if (!collision1 && !collision2)
					continue; // 충돌이 일어나지 않는다면, 무시

				// 현재 노드와 다음 노드에서 중복된 버텍스 2개에서, (인접 버텍스)
				// 인접하지 않는 변에 속하는 버텍스가, 충돌되는 꼭지점이다.
				// 이 버텍스를 중심으로 다시 최적 경로를 탐색한다.
				int pointIdx1, pointIdx2;
				if (collision1)// || (curVtxEdge1 < 0))
				{
					pointIdx1 = GetAdjacentCollisionVertexIdx(idxs[k], idxs[(k + 1) % 3], nodeIndices, false);
					pointIdx2 = (pointIdx1 == idxs[k]) ? idxs[(k + 1) % 3] : idxs[k];
				}
				else
				{
					pointIdx1 = GetAdjacentCollisionVertexIdx(curVtxEdge1, curVtxEdge2, nodeIndices, true);
					pointIdx2 = (pointIdx1 == curVtxEdge1) ? curVtxEdge2 : curVtxEdge1;
				}

				if (pointIdx1 < 0)
				{
					assert(pointIdx1 >= 0);
					break;
				}

				//if (pointIdx1 == curVtxEdge1)
				//	continue; // 중복 버텍스, 무시

				if (collision1)
				{
					const Vector3 dir = (m_vertices[pointIdx1] - m_vertices[pointIdx2]).Normal();
					curPos = m_vertices[pointIdx1] + dir*0.1f + tri1.Normal() * -0.1f + Vector3(0, 1, 0);

					turnTri1 = tri1;
					turnTri2 = tri2;
				}
				else // collision2
				{
					const Vector3 dir = (m_vertices[pointIdx1] - m_vertices[pointIdx2]).Normal();
					curPos = m_vertices[pointIdx1] + dir*0.1f + turnTri1.Normal() * -0.1f + Vector3(0, 1, 0);
				}

				curVtxIdx = pointIdx1;
				curVtxEdge1 = pointIdx1;
				curVtxEdge2 = pointIdx2;

				s = GetNearestNodeFromVertexIdx(nodeIndices, curVtxEdge1, curVtxEdge2).second;
				out.push_back(curPos - Vector3(0,1,0));

				break;
			} // for k
		} // for o
	} // for i

	out.push_back(path.back());
}


// nodeFromIdx 노드와 nodeToIdx 노드의 인접 버텍스에서
// adjVtxIdx1, adjVtxIdx2 버텍스와 중복된 Vertex Index를 리턴한다.
// 없다면 -1을 리턴한다.
int cNavigationMesh::GetAdjacentCollisionVertexIdx(
	const int adjVtxIdx1, const int adjVtxIdx2
	, const int nodeFromIdx, const int nodeToIdx)
{
	sNaviNode &curNode = m_naviNodes[nodeFromIdx];

	const int idxs[3] = { curNode.idx1, curNode.idx2, curNode.idx3 };
	for (int i = 0; i < 3; ++i)
	{
		if (nodeToIdx == curNode.adjacent[i])
		{
			if (adjVtxIdx1 == idxs[i])
				return adjVtxIdx1;
			if (adjVtxIdx1 == idxs[(i + 1) % 3])
				return adjVtxIdx1;
			if (adjVtxIdx2 == idxs[i])
				return adjVtxIdx2;
			if (adjVtxIdx2 == idxs[(i + 1) % 3])
				return adjVtxIdx2;
		}
	}

	return -1;
}


// 경로상에서 adjVtxIdx1, adjVtxIdx2와 겹치는 노드에서, 가장먼저 연결된
// Vertex Index를 리턴한다.
int cNavigationMesh::GetAdjacentCollisionVertexIdx(const int adjVtxIdx1, const int adjVtxIdx2
	, const vector<int> &nodeIndices
	, const bool isReverse //= true
)
{
	if (isReverse)
	{
		for (int i = (int)nodeIndices.size() - 1; i >= 0; --i)
		{
			const sNaviNode &node = m_naviNodes[nodeIndices[i]];
			if ((node.idx1 == adjVtxIdx1)
				|| (node.idx2 == adjVtxIdx1)
				|| (node.idx3 == adjVtxIdx1))
				return adjVtxIdx1;

			if ((node.idx1 == adjVtxIdx2)
				|| (node.idx2 == adjVtxIdx2)
				|| (node.idx3 == adjVtxIdx2))
				return adjVtxIdx2;
		}
	}
	else
	{
		for (int i = 0; i < (int)nodeIndices.size(); ++i)
		{
			const sNaviNode &node = m_naviNodes[nodeIndices[i]];
			if ((node.idx1 == adjVtxIdx1)
				|| (node.idx2 == adjVtxIdx1)
				|| (node.idx3 == adjVtxIdx1))
				return adjVtxIdx1;

			if ((node.idx1 == adjVtxIdx2)
				|| (node.idx2 == adjVtxIdx2)
				|| (node.idx3 == adjVtxIdx2))
				return adjVtxIdx2;
		}
	}

	return -1;
}


// nodeIndices에서 vtxIdx 가 포함된 node index 를 리턴한다.
// 이 때, 가장 처음의 node index 를 리턴한다.
// 없다면, -1을 리턴한다.
// return = {node index, order of nodeIndices}
std::pair<int, int> cNavigationMesh::GetNearestNodeFromVertexIdx(const vector<int> &nodeIndices
	, const int vtxIdx)
{
	for (u_int i = 0; i < nodeIndices.size(); ++i)
	{
		const sNaviNode &node = m_naviNodes[nodeIndices[i]];
		if ((node.idx1 == vtxIdx)
			|| (node.idx2 == vtxIdx)
			|| (node.idx3 == vtxIdx))
		{
			return{ nodeIndices[i], i };
		}
	}

	return{ -1,-1 };
}

// nodeIndices에서 vtxIdx1, vtxIdx2 가 포함된 node index 를 리턴한다.
// 이 때, 가장 처음의 node index 를 리턴한다.
// 없다면, -1을 리턴한다.
// return = {node index, order of nodeIndices}
std::pair<int, int> cNavigationMesh::GetNearestNodeFromVertexIdx(const vector<int> &nodeIndices
	, const int vtxIdx1, const int vtxIdx2)
{
	for (u_int i = 0; i < nodeIndices.size(); ++i)
	{
		const sNaviNode &node = m_naviNodes[nodeIndices[i]];
		if (
			((node.idx1 == vtxIdx1) || (node.idx2 == vtxIdx1) || (node.idx3 == vtxIdx1))
			&& ((node.idx1 == vtxIdx2) || (node.idx2 == vtxIdx2) || (node.idx3 == vtxIdx2))
			)
		{
			return{ nodeIndices[i], i };
		}
	}
	return{ -1,-1 };
}


// 두 노드 nodeIndex1, nodeIndex2 의 인접한 버텍스 2개를 리턴한다.
// 없다면 -1을 리턴한다.
std::pair<int, int> cNavigationMesh::GetAdjacentVertexIdx(
	const int nodeIndex1, const int nodeIndex2)
{
	sNaviNode &node1 = m_naviNodes[nodeIndex1];
	sNaviNode &node2 = m_naviNodes[nodeIndex2];

	const int idxs[3] = { node1.idx1, node1.idx2, node1.idx3 };
	for (int i = 0; i < 3; ++i)
	{
		if (node1.adjacent[i] == nodeIndex2)
		{
			return { idxs[i], idxs[(i + 1) % 3] };
		}
	}

	return {-1, -1};
}
