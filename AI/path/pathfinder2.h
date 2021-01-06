//
// 2020-12-30, jjuiddong
// pathfinder2
//	- upgrade pathfinder
//
#pragma once


namespace ai
{

	class cPathFinder2
	{
	public:
		struct sTransition
		{
			int to; // link vertex index
			float distance; // two vertex distance
			float w; // weight
			int prop; // property, bidirection?
			int toWaypoint; // direction waypoint id
			bool enable; // transition enable/disable
		};

		struct sVertex
		{
			int id; // calc from name, atoi()
			int type;
			Str16 name; // number string
			Vector3 pos;
			vector<sTransition> trs;
			bool visit; // use internal
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

		typedef vector<Vector3> ppath;	// position path
		typedef vector<uint> vpath;		// vertex index path
		typedef vector<sEdge> epath;	// edge path


		cPathFinder2();
		virtual ~cPathFinder2();

		bool Find(const Vector3 &start, const Vector3 &end
			, OUT vector<Vector3> &out
			, const set<sEdge> *disableEdges = nullptr
			, OUT vector<ushort> *outTrackVertexIndices = nullptr
			, OUT vector<sEdge> *outTrackEdges = nullptr
		);

		bool Find(const uint startIdx, const uint endIdx
			, OUT vector<Vector3> &out
			, const set<sEdge> *disableEdges = nullptr
			, OUT vector<ushort> *outTrackVertexIndices = nullptr
			, OUT vector<sEdge> *outTrackEdges = nullptr
		);

		bool Find(const uint startIdx, const uint endIdx
			, OUT vector<ushort> &out
			, const set<sEdge> *disableEdges = nullptr
		);

		int GetNearestVertex(const Vector3 &pos) const;

		uint AddVertex(const sVertex &vtx);
		bool AddTransition(const uint fromVtxIdx, const uint toVtxIdx
			, const int prop = 0);
		bool IsExistTransition(const uint fromVtxIdx, const uint toVtxIdx);
		bool RemoveTransition(const uint fromVtxIdx, const uint toVtxIdx);
		bool RemoveVertex(const uint vtxIdx);
		sVertex* GetVertexByIndex(const uint vtxIdx);
		sVertex* GetVertexByName(const Str16 &name);
		sVertex* GetVertexByID(const int id);
		int GetVertexIndexByName(const Str16 &name) const;
		int GetVertexIndexByID(const int id);
		void ReservedVertexBuffer(const uint vertexCount);
		void Clear();


	public:
		vector<sVertex> m_vertices;
		map<int, uint> m_vtxMap; // vertex id-index mapping
								 // to fast search vertex array
								 // key: vertex name (number only)
							     // value: m_vertices index
	};

}
