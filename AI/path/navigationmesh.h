//
// 2017-12-03, jjuiddong
// Navigation Mesh
//
#pragma once


namespace ai
{
	class cPathFinder;

	class cNavigationMesh
	{
	public:
		cNavigationMesh();
		virtual ~cNavigationMesh();

		bool ReadFromPathFile(const char *fileName);


	protected:
		int BuildPolygonFromPathFinder(const cPathFinder &pathFinder
			, const int vtxIdx1, const int vtxIdx2j, INOUT vector<WORD> &indices);
		bool IsExistTriangle(const vector<WORD> &indices
			, const int vtxIdx1, const int vtxIdx2, const int vtxIdx3);


	public:
		struct sNaviNode
		{
			int idx1, idx2, idx3; // Vertex Index (m_vertices)
			Vector3 center;
			int adjacent[3]; // adjacent NaviNode index (m_naviNodes)
		};

		vector<Vector3> m_vertices;
		vector<sNaviNode> m_naviNodes;
	};

}
