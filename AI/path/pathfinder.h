//
// 2017-06-19, jjuiddong
// path finder
// Does Not Multi Thread Safe
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
			enum { MAX_EDGE = 10, MAX_VERTEX = 1024 };

			int type; // 0:path point, 1:destination1, 2:destination2, 3:temporary node
			Vector3 pos;
			int edge[MAX_EDGE];
			float startLen;
			float endLen;

			sVertex() : type(0)
			{
				for (int i = 0; i < MAX_EDGE; ++i)
					edge[i] = -1;
			}
		};

		struct sEdge
		{
			int from, to;
		
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
		void Clear();


	protected:
		float Distance_Manhatan(const Vector3 &p0, const Vector3 &p1) const;
		float Distance(const Vector3 &p0, const Vector3 &p1) const;


	public:
		vector<sVertex> m_vertices;
		vector<sArea> m_areas;
	};


	// search table
	extern float g_edges_len[cPathFinder::sVertex::MAX_VERTEX][cPathFinder::sVertex::MAX_VERTEX];
	extern bool  g_edges_visit[cPathFinder::sVertex::MAX_VERTEX][cPathFinder::sVertex::MAX_VERTEX];
}
