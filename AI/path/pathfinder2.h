//
// 2020-12-30, jjuiddong
// pathfinder2
//	- upgrade pathfinder
//
// 2022-12-08
//	- add targetting move algorithm
// 
// 2023-01-10
//	- rename transition -> edge
//	- curve edge information
//
#pragma once


namespace ai
{

	class cPathFinder2
	{
	public:
		struct sEdge
		{
			uint to; // link vertex index
			float distance; // two vertex distance
			float w; // weight
			float speed; // maximum speed
			int prop; // property, bidirection?
			int toWaypoint; // direction waypoint id
			bool enable; // edge enable/disable

			// direction
			Vector3 dir0; // out direction from current vertex
			Vector3 dir1; // in direction to next to-vertex

			// curve edge information
			bool isCurve; // curve edge?
			float curveAngle; // bezier curve angle, opengl space, 0>angle:left, 0>angle:right
			float curveDist; // bezier two control point distance
			int dirFrVtxIdx; // edge from vertex index to determine curve direction
			vector<Vector3> ctrlPts; // quadratic bezier curve control position list size = 4
			vector<Vector3> bezier; // bezier curve position list
			vector<float> bezierLens; // bezier curve length list

			sEdge() : isCurve(false) {}
		};

		struct sVertex
		{
			int id; // calc from name, atoi()
			int type;
			Str16 name; // number string
			Vector3 pos;
			vector<sEdge> edges;
			bool avoidable; // is avoidable?
			bool visit; // use internal
			int userData; // custom data

			sVertex() : avoidable(true) {}
		};

		struct sEdge2
		{
			int from, to; // sVertex Index

			sEdge2() : from(0), to(0) {}
			sEdge2(int _from, int _to) : from(_from), to(_to) {}
			bool operator==(const sEdge2&rhs) const {
				return (from == rhs.from) && (to == rhs.to);
			}
			bool operator<(const sEdge2&rhs) const {
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
			, const set<sEdge2> *disableEdges = nullptr
			, OUT vector<uint> *outTrackVertexIndices = nullptr
			, OUT vector<sEdge2> *outTrackEdges = nullptr
		);

		bool Find(const uint startIdx, const uint endIdx
			, OUT vector<Vector3>& out
			, const set<sEdge2>* disableEdges = nullptr
			, OUT vector<uint>* outTrackVertexIndices = nullptr
			, OUT vector<sEdge2>* outTrackEdges = nullptr
		);

		bool Find(const uint startIdx, const uint endIdx
			, OUT vector<uint>& out
			, const set<sEdge2>* disableEdges = nullptr
			, const bool isChangeDirPenalty = false
			, const sTarget* target = nullptr
			, const float rotationLimit = -2.f
		);

		int GetNearestVertex(const Vector3 &pos) const;
		bool CreateEdgeMap();

		uint AddVertex(const sVertex &vtx);
		bool AddEdge(const uint fromVtxIdx, const uint toVtxIdx
			, const int prop = 0, const float speed = 0.f);
		bool SetCurveEdge(const uint fromVtxIdx, const uint toVtxIdx
			, const bool isCurve, const float angle, const float dist, const float arcLen
			, const uint directionFrVtxIdx);
		bool IsExistEdge(const uint fromVtxIdx, const uint toVtxIdx);
		bool RemoveEdge(const uint fromVtxIdx, const uint toVtxIdx);
		bool RemoveVertex(const uint vtxIdx);
		sVertex* GetVertexByIndex(const uint vtxIdx);
		sVertex* GetVertexByName(const Str16 &name);
		int GetVertexIndexByName(const Str16 &name) const;
		void ReservedVertexBuffer(const uint vertexCount);
		void Clear();

		static float GetCurveEdgeDistance(const sEdge& edge, const Vector3& pos
			, OUT float* ratio =nullptr);
		static bool GetCurveEdgeMove(const sEdge& edge, const Vector3& pos
			, const float dist, OUT Vector3& outPos, OUT Vector3& outDir
			, OUT float* outRatio =nullptr);
		static bool GetCurveEdgeFirstPos(const sEdge& edge
			, const Vector3& pos
			, const Vector3& dir
			, const float dist
			, OUT Vector3& outPos
			, OUT Vector3& outDir);
		static bool IsOnEdge(const sEdge& edge, const Vector3& pos
			, const float offset = 0.05f);


	protected:
		inline int ArriveTarget(sTargetArg &arg);

	public:
		vector<sVertex> m_vertices;

		// fast search from-to edge data, generate from CreateEdgeMap
		// key: from-to vertex index, value: edge
		map<std::pair<uint, uint>, sEdge*> m_edgeMap;
	};

}
