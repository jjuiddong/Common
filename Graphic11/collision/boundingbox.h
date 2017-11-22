// 
// 2017-08-02, jjuiddong
// OBB (Oriendted Bounding Box)
//
//
//	BoundingOrientedBox
//		- XMFLOAT3 Center;            // Center of the box.
//		- XMFLOAT3 Extents;           // Distance from the center to each side.
//		- XMFLOAT4 Orientation;       // Unit quaternion representing rotation (box -> world).
//
#pragma once


namespace graphic
{
	class cCube;
	class cBoundingSphere;

	class cBoundingBox
	{
	public:
		cBoundingBox();
		cBoundingBox(const cCube &cube);
		cBoundingBox(const Vector3 &center, const Vector3 &scale, const Quaternion &q);
		void SetBoundingBox(const Vector3 &center, const Vector3 &scale, const Quaternion &q);
		void SetBoundingBox(const Transform &tfm);
		void SetBoundingBox(const sMinMax &minMax );
		void SetLineBoundingBox(const Vector3 &p0, const Vector3 &p1, const float width);
		bool Collision( cBoundingBox &box );
		bool Collision( cBoundingSphere &sphere);
		bool Collision2D(cBoundingSphere &sphere, OUT Vector3 *out=NULL);

		bool Pick(const Vector3 &orig, const Vector3 &dir, OUT float *distance=NULL);
		bool Pick(const Ray &ray, OUT float *distance = NULL);

		Vector3 Center() const;
		void Scale(const Vector3 &scale);
		Vector3 GetDimension() const;
		XMMATRIX GetMatrixXM() const;
		Matrix44 GetMatrix() const;
		Transform GetTransform() const;
		Vector3 BoundingPoint(const Vector3 &pos);

		cBoundingBox operator * (const XMMATRIX &rhs);
		cBoundingBox operator * (const Matrix44 &rhs);
		const cBoundingBox& operator *= (const Matrix44 &rhs);
		const cBoundingBox& operator *= (const XMMATRIX &rhs);
		cBoundingBox& operator=(const cCube &cube);


	public:
		BoundingOrientedBox m_bbox;
	};

}
