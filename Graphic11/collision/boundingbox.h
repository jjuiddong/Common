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


namespace common
{
	//class cCube;
	class cBoundingSphere;

	class cBoundingBox : public cCollisionObj
	{
	public:
		cBoundingBox();
		//cBoundingBox(const cCube &cube);
		cBoundingBox(const Vector3 &center, const Vector3 &scale, const Quaternion &q);
		void SetBoundingBox(const Vector3 &center, const Vector3 &scale, const Quaternion &q);
		void SetBoundingBox(const Transform &tfm);
		void SetBoundingBox(const sMinMax &minMax );
		void SetLineBoundingBox(const Vector3 &p0, const Vector3 &p1, const float width);
		bool Collision2D(cBoundingSphere &sphere
			, OUT Vector3 *outPos=NULL
			, OUT Plane *outPlane=NULL
			, OUT Vector3 *outVertex1=NULL
			, OUT Vector3 *outVertex2=NULL
		);

		virtual bool Collision(const cCollisionObj &obj
			, OUT Vector3 *outPos = NULL, OUT float *distance = NULL) const override;
		virtual bool Pick(const Ray &ray, OUT float *distance = NULL) const override;
		bool Pick(const Vector3 &orig, const Vector3 &dir, OUT float *distance=NULL) const;

		Vector3 Center() const;
		void Scale(const Vector3 &scale);
		float GetScale() const;
		Vector3 GetDimension() const;
		XMMATRIX GetMatrixXM() const;
		Matrix44 GetMatrix() const;
		virtual Transform GetTransform() const override;
		Vector3 GetBoundingPoint(const Vector3 &pos) const;
		void GetVertexPoint(OUT Vector3 out[8]) const;

		cBoundingBox operator + (const cBoundingBox &rhs);
		cBoundingBox operator * (const XMMATRIX &rhs);
		cBoundingBox operator * (const Matrix44 &rhs);
		cBoundingBox& operator += (const cBoundingBox &rhs);
		cBoundingBox& operator *= (const Matrix44 &rhs);
		cBoundingBox& operator *= (const XMMATRIX &rhs);
		//cBoundingBox& operator=(const cCube &cube);


	public:
		BoundingOrientedBox m_bbox;
	};

}
