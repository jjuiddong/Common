//
// 2017-06-19, jjuiddong
// path finder
//	- Thread Safe
//
// 2017-11-09, jjuiddong
//	- Path Find, disable edge list
//
// 2018-11-10, jjuiddong
//	- thread safe
//	- edge weight
//
// 2018-12-20, jjuiddong
//	- refactoring
//
//
#pragma once


namespace ai
{
	using namespace common;

	class cPathFinder
	{
	public:
		struct sVertex
		{
			struct sEdge
			{
				int to; // link next vertex index
				float distance; // edge distance
				float w; // edge weight
				bool enable; // edge enable/disable
			};

			enum { MAX_EDGE = 10, MAX_VERTEX = 1024 };
			int type; // 0:path point, 1:destination1, 2:destination2, 3:temporary node
			Vector3 pos;
			sEdge edge[MAX_EDGE];
			//int edge[MAX_EDGE];
			//float w[MAX_EDGE]; // edge weight
			int data[4];
			int replaceFromIdx; // using temporary vertex, replace vertex from, to
			int replaceToIdx;
			float startLen;
			float endLen;

			sVertex() : type(0) {
				for (int i = 0; i < MAX_EDGE; ++i)
					edge[i].to = -1;
				//for (int i = 0; i < MAX_EDGE; ++i)
				//	edge[i] = -1;
				//for (int i = 0; i < MAX_EDGE; ++i)
				//	w[i] = 1.f;
				for (int i = 0; i < ARRAYSIZE(data); ++i)
					data[i] = -1;
				replaceFromIdx = -1;
				replaceToIdx = -1;
			}
		};

		struct sEdge
		{
			int from, to; // sVertex Index
		
			sEdge() : from(0), to(0) {}
			sEdge(int _from, int _to) : from(_from), to(_to) {}
			bool operator==(const sEdge &rhs) const {
				return (from == rhs.from) && (to == rhs.to);
			}
			bool operator<(const sEdge &rhs) const {
				return (from < rhs.from) || ((from == rhs.from) && (to < rhs.to));
			}		
		};

		struct sArea
		{
			int id;
			sRectf rect;
			vector<int> indices; // sVertex Indices
		};

		typedef vector<Vector3> ppath;	// position path
		typedef vector<u_int> vpath;	// vertex index path
		typedef vector<sEdge> epath;	// edge path

		cPathFinder();
		virtual ~cPathFinder();
		bool Create(const int vertexCount);
		bool Read(const StrPath &fileName);
		bool Write(const StrPath &fileName);
		bool Find(const Vector3 &start, const Vector3 &end
			, OUT vector<Vector3> &out
			, OUT vector<int> *outTrackVertexIndices = NULL
			, OUT vector<sEdge> *outTrackEdges = NULL
			, const set<sEdge> *disableEdges = NULL
		);

		bool AddVertex(const sVertex &vtx);
		bool AddEdge(const int vtxIdx, const int addEdgeIdx);
		bool RemoveEdge(const int vtxIdx, const int removeEdgeIdx);
		bool RemoveEdgeEachOther(const int vtxIdx, const int removeEdgeIdx);
		bool RemoveVertex(const int index);
		bool AddArea(const sRectf &area);
		int GetNearestVertex(const Vector3 &pos) const;
		int GetNearestVertex(const Vector3 &pos, const Vector3 &end) const;
		int GetNearestArea(const Vector3 &pos) const;
		std::pair<int,int> GetNearestEdge(const Vector3 &pos) const;
		static int MakeEdgeKey(const int from, const int to);
		void Clear();


	protected:
		void OptimizeAreaPath(const Vector3 &start
			, const Vector3 &end
			, INOUT vector<Vector3> &out
			, INOUT vector<int> &verticesIndices
		);		
		float Distance_Manhatan(const Vector3 &p0, const Vector3 &p1) const;
		float Distance(const Vector3 &p0, const Vector3 &p1) const;


	public:
		vector<sVertex> m_vertices;
		vector<sArea> m_areas;
		map<int, float> m_lenSet; // key = edgeKey, data = length, for debugging
	};

}
