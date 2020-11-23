//
// 2018-11-02, jjuiddong
//	2 Dimensional map path finder
//	thread safe
//
//	- node character type
//		- 0 : no move
//		- 1 : slow node
//		- 2 : fast node
//		- 3 : wide road node
//
// sample data
//
// 111111111111111111111111
// 122222222222222222222222
// 120111102011110201111021
// 121000012100001210000121
// 121000012100001210000121
// 121000012100001210000121
// 121000012100001210000121
// 121000012100001210000121
// 121000012100001210000121
// 121000012100001210000121
// 121000012100001210000121
// 121000012100001210000121
// 121000012100001210000121
// 121000012100001210000121
// 120111102011110201111021
// 122222222222222222222222
// 111111111111111111111111
//
#pragma once


namespace ai
{

	class cPathFinder2D
	{
	public:
		struct sVertex
		{
			int type;
			int edgeKey; // m_fastmap unique edge key (= max(from,to)*1000 + min(from,to))
			float startLen;
			float endLen;
			sVertex() : type(0) {}
		};

		typedef vector<Vector2i> ppath;	// position path
		typedef vector<uint> vpath;	// vertex index path
		typedef vector<cPathFinder::sEdge> epath; // edge path

		cPathFinder2D();
		virtual ~cPathFinder2D();

		bool Read(const char *fileName);
		bool Find(const Vector2i &startPos
			, const Vector2i &endPos
			, OUT ppath &out);
		bool FindEnumeration(const Vector2i &startPos
			, const Vector2i &endPos
			, OUT vector<ppath> &outPos
			, OUT vector<ppath> &outVertexPos
			, OUT vector<epath> &outEdges);
		bool GetEdgePath2PosPath(const epath &edgePath, OUT ppath &out);
		inline int MakeEdgeKey(const int from, const int to) const;
		inline int MakeUniqueEdgeKey(const int from, const int to) const;
		inline std::pair<int, int> SeparateEdgeKey(const int edgeKey) const;
		void Clear();


	protected:
		inline sVertex& GetMap(const int idx);
		inline sVertex& GetMap(const Vector2i &pos);
		std::pair<int, int> GetRowsCols(const char *fileName);
		bool GenerateGraphNode();
		bool AddTemporaryVertexAndEdges(const Vector3 &pos);
		bool RemoveTemporaryVertexAndEdges(const Vector3 &pos);
		bool CollectEdgeVertices(const int from, const int to, const int uniqueEdgeKey
			, OUT ppath &out);
		bool CollectEdgeAddedVertices(const int from, const int to, const int uniqueEdgeKey
			, const int addedVertexFrom, const int addedVertexTo
			, OUT ppath &out);

		inline bool CheckRange(const Vector2i &pos);
		float Distance_Manhatan(const Vector2i &p0, const Vector2i &p1) const;


	public:
		sVertex *m_map; // size = m_rows*m_cols, access = y*m_cols + x
		cPathFinder *m_fastmap; // fast path search
		int m_rows;
		int m_cols;
		vector<Vector2i> m_waypoints;
		static sVertex m_dummy;
	};

}
