//
// 2020-12-30, jjuiddong
// pathfinder2
//	- upgrade pathfinder
//
// 2022-12-08
//	- add targetting move algorithm
//
#pragma once


namespace ai
{

	class cPathFinder2
	{
	public:
		struct sTransition
		{
			uint to; // link vertex index
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
			bool avoidable; // is avoidable?
			bool visit; // use internal

			sVertex() : avoidable(true) {}
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

		// path finding object target information
		struct sTarget
		{
			float radius; // target range
			Vector3 target; // target position
			Vector3 dir; // initial object direction
		};

		// arrive target process argument
		struct sTargetArg
		{
			uint startIdx; // start vertex index
			uint endIdx; // destination vertex index
			uint from; // from vertex index
			uint to; // to vertex index
			float targetDist; // object to target distance (to optimize)
			const sTarget* target;
		};

		cPathFinder2();
		virtual ~cPathFinder2();

		bool Find(const Vector3 &start, const Vector3 &end
			, OUT vector<Vector3> &out
			, const set<sEdge> *disableEdges = nullptr
			, OUT vector<uint> *outTrackVertexIndices = nullptr
			, OUT vector<sEdge> *outTrackEdges = nullptr
		);

		bool Find(const uint startIdx, const uint endIdx
			, OUT vector<Vector3>& out
			, const set<sEdge>* disableEdges = nullptr
			, OUT vector<uint>* outTrackVertexIndices = nullptr
			, OUT vector<sEdge>* outTrackEdges = nullptr
		);

		bool Find(const uint startIdx, const uint endIdx
			, OUT vector<uint> &out
			, const set<sEdge> *disableEdges = nullptr
			, const bool isChangeDirPenalty = false
			, const sTarget *target = nullptr
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
		int GetVertexIndexByName(const Str16 &name) const;
		void ReservedVertexBuffer(const uint vertexCount);
		void Clear();


	protected:
		inline int ArriveTarget(sTargetArg &arg);

	public:
		vector<sVertex> m_vertices;
	};

}
