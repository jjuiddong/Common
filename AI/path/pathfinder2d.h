//
// 2018-11-02, jjuiddong
// 2 Dimensional map path finder
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
	class cPathFinder;

	class cPathFinder2D
	{
	public:
		struct sVertex
		{
			int type;
			float startLen;
			float endLen;
			sVertex() : type(0) {}
		};

		cPathFinder2D();
		virtual ~cPathFinder2D();

		bool Read(const char *fileName);
		bool Find(const Vector2i &startPos
			, const Vector2i &endPos
			, OUT vector<Vector2i> &out);
		bool FindEnumeration(const Vector2i &startPos
			, const Vector2i &endPos
			, OUT vector<vector<Vector2i>> &out);
		void Clear();


	protected:
		inline sVertex& GetMap(const int idx);
		inline sVertex& GetMap(const Vector2i &pos);
		std::pair<int, int> GetRowsCols(const char *fileName);
		bool GenerateGraphNode();
		bool AddTemporaryVertexAndEdges(const Vector3 &pos);
		bool RemoveTemporaryVertexAndEdges(const Vector3 &pos);

		inline bool CheckRange(const Vector2i &pos);
		float Distance_Manhatan(const Vector2i &p0, const Vector2i &p1) const;


	public:
		sVertex *m_map; // size = m_rows*m_cols, access = y*m_cols + x
		cPathFinder *m_graph; // fast path search
		int m_rows;
		int m_cols;
		vector<Vector2i> m_waypoints;
		static sVertex m_dummy;
	};

}
