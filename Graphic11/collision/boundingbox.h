// 
// 2017-08-02, jjuiddong
// OBB (Oriendted Bounding Box)
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
		cBoundingBox(const Vector3 &center, const Vector3 &scale, const Quaternion &q=Quaternion());
		void SetBoundingBox(const Vector3 &center, const Vector3 &scale, const Quaternion &q = Quaternion());
		void SetLineBoundingBox(const Vector3 &p0, const Vector3 &p1, const float width);
		bool Collision( cBoundingBox &box );
		bool Pick(const Vector3 &orig, const Vector3 &dir, OUT float *distance=NULL);
		Vector3 Center() const;
		void Scale(const Vector3 &scale);
		Vector3 GetDimension() const;
		void Transform(const Matrix44 &tm);
		XMMATRIX GetTransform();

		cBoundingBox& operator=(const cCube &cube);


	public:
		BoundingOrientedBox m_bbox;
	};

}
