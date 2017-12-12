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
		bool Find(const Vector3 &start, const Vector3 &end
			, OUT vector<Vector3> &out1
			, OUT vector<Vector3> &out2);


	protected:
		int BuildPolygonFromPathFinder(const cPathFinder &pathFinder
			, const int vtxIdx1, const int vtxIdx2, const int vtxIdx3, INOUT vector<WORD> &indices);
		int GetExistTriangle(const vector<WORD> &indices
			, const int vtxIdx1, const int vtxIdx2, const int vtxIdx3);
		int GetNearestNode(const Vector3 &pos);
		int GetAdjacentCollisionVertexIdx(const int adjVtxIdx1, const int adjVtxIdx2
			, const int nodeFromIdx, const int nodeToIdx);
		int GetAdjacentCollisionVertexIdx(const int adjVtxIdx1, const int adjVtxIdx2
			, const vector<int> nodeIndices, const bool isReverse=true);

		std::pair<int,int> GetNearestNodeFromVertexIdx(const vector<int> nodeIndices, const int vtxIdx);
		std::pair<int, int> GetNearestNodeFromVertexIdx(const vector<int> nodeIndices
			, const int vtxIdx1, const int vtxIdx2);

		void OptimizePath(const vector<int> &nodeIndices
			, const vector<Vector3> &path
			, OUT vector<Vector3> &out);


	public:
		struct sNaviNode
		{
			int idx1, idx2, idx3; // Vertex Index (m_vertices)
			Vector3 center;
			int adjacent[3]; // adjacent NaviNode index (m_naviNodes)
			float startLen; // for search
			float endLen; // for search
		};

		vector<Vector3> m_vertices;
		vector<sNaviNode> m_naviNodes;
	};

}
