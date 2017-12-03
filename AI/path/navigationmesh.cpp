
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
		BuildPolygonFromPathFinder(pathFinder, i, vtx.edge[0], indices);
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
	, const int vtxIdx1, const int vtxIdx2
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
				const int vtxIdx3 = vtx1.edge[i];
				if (IsExistTriangle(indices, vtxIdx1, vtxIdx2, vtxIdx3))
					continue; // 이미 등록된 폴리곤이라면, 무시

				// vtxIdx1, vtxIdx2가 포함된 폴리곤 나머지 버텍스를 찾음.
				// 왼손 법칙을 이용해 Normal이 Vector3(0,1,0)이 되는 순서로 저장한다.
				const Vector3 p1 = vtx1.pos;
				const Vector3 p2 = vtx2.pos;
				const Vector3 p3 = pathFinder.m_vertices[vtxIdx3].pos;
				Triangle tri(p1, p2, p3);
				if (tri.Normal().y > 0.f)
				{
					indices.push_back(vtxIdx1);
					indices.push_back(vtxIdx2);
					indices.push_back(vtxIdx3);
				}
				else
				{
					indices.push_back(vtxIdx1);
					indices.push_back(vtxIdx3);
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
	const int idx1 = BuildPolygonFromPathFinder(pathFinder, polyIndices[0], polyIndices[1], indices);
	const int idx2 = BuildPolygonFromPathFinder(pathFinder, polyIndices[1], polyIndices[2], indices);
	const int idx3 = BuildPolygonFromPathFinder(pathFinder, polyIndices[2], polyIndices[0], indices);

	m_naviNodes[nodeId].adjacent[0] = idx1;
	m_naviNodes[nodeId].adjacent[1] = idx2;
	m_naviNodes[nodeId].adjacent[2] = idx3;

	return nodeId;
}


// 등록된 폴리곤이 있는지 검색한다. 있다면 true를 리턴한다.
bool cNavigationMesh::IsExistTriangle(const vector<WORD> &indices
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
			return true;
	}
	return false;
}
