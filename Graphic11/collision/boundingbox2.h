//
// 2017-06-16, jjuiddong 
// Line BoundingBox
//
//                 (1,1,1)
//       3-----2
//      /|      /|
//     1-----0 |
//	   | 7 -  | 6
//	   |/      |/
//	   5-----4
//	(-1,-1,-1)
//
#pragma once


namespace graphic
{

	class cBoundingBox2
	{
	public:
		cBoundingBox2();
		void SetLineBoundingBox(const Vector3 &p0, const Vector3 &p1, const float width);


		Matrix44 m_tm;
	};

}
