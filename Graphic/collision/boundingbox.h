// 
// OBB (Oriendted Bounding Box)
//	min + max + matrix44
//
#pragma once


namespace graphic
{
	class cCube;

	class cBoundingBox
	{
	public:
		cBoundingBox();
		cBoundingBox(const cCube &cube);
		void SetBoundingBox(const Vector3 &vMin, const Vector3 &vMax );
		void SetLineBoundingBox(const Vector3 &p0, const Vector3 &p1, const float width);
		void SetTransform( const Matrix44 &tm );
		bool Collision( cBoundingBox &box );
		bool Pick(const Vector3 &orig, const Vector3 &dir, const Matrix44 &tm = Matrix44::Identity);
		bool Pick2(const Vector3 &orig, const Vector3 &dir, float *pfT, float *pfU, float *pfV);
		bool Pick3(const Vector3 &orig, const Vector3 &dir, float *pDistance, const Matrix44 &tm = Matrix44::Identity);
		float Length() const;
		Vector3 Center() const;
		Vector3 Center2() const;
		void Scale(const Vector3 &scale);
		Vector3 GetDimension();

		cBoundingBox& operator=(const cCube &cube);


	public:
		Vector3 m_min;
		Vector3 m_max;
		Matrix44 m_tm;
	};


	inline float cBoundingBox::Length() const { return (m_min - m_max).Length(); }
}
