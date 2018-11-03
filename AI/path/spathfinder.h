//
// 2018-11-02, jjuiddong
// simple AStar path finder
//	- 2 dimensional map path finder
//
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

	class cSPathFinder
	{
	public:
		cSPathFinder();
		virtual ~cSPathFinder();

		bool Read(const char *fileName);
		bool Find(const Vector2i &startPos
			, const Vector2i &endPos
			, OUT vector<Vector2i> &out);
		void Clear();


	protected:
		std::pair<int, int> GetRowsCols(const char *fileName);
		float Distance_Manhatan(const Vector2i &p0, const Vector2i &p1) const;


	public:
		struct sVertex
		{
			int type;
			float startLen;
			float endLen;
			sVertex() :type(0)
			{
			}
		};

		sVertex *m_map; // y*m_cols + x
		int m_rows;
		int m_cols;
	};

}
